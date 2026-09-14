#include "state_manager.h"
#include "esphome/core/log.h"
#include <time.h>

namespace esphome {
namespace automate {

static const char* const TAG = "state_mgr";

void StateManager::setup() {
  this->load_desired_state();
  for (uint8_t i = 0; i < RELAY_COUNT; i++) {
    maint_modes_[i] = MAINTENANCE_AUTO;
  }
  this->load_scenes();
  initialized_ = true;
  ESP_LOGI(TAG, "State manager initialized");
}

void StateManager::dump_config() {
  ESP_LOGCONFIG(TAG, "AutoMate State Manager");
}

void StateManager::set_desired_pwm(PwmId id, float value) {
  if (id >= PWM_COUNT) return;
  value = std::max(0.0f, std::min(1.0f, value));
  desired_state_.pwm_values[id] = value;
  this->save_desired_state();
  this->update_effective_outputs();
}

void StateManager::set_desired_relay(RelayId id, bool value) {
  if (id >= RELAY_COUNT) return;
  if (maint_modes_[id] == MAINTENANCE_FORCE_OFF && value) {
    ESP_LOGW(TAG, "Relay %d blocked by FORCE_OFF", id);
    return;
  }
  if (maint_modes_[id] == MAINTENANCE_FORCE_ON && !value) {
    ESP_LOGW(TAG, "Relay %d blocked by FORCE_ON", id);
    return;
  }
  if (id == RELAY_CEILING || id == RELAY_12V || id == RELAY_STABILIZER) {
    if (!this->check_interlocks(id, value)) {
      ESP_LOGW(TAG, "Relay %d blocked by interlock", id);
      return;
    }
  }
  desired_state_.relay_values[id] = value;
  this->save_desired_state();
  this->update_effective_outputs();
}

float StateManager::get_desired_pwm(PwmId id) const {
  if (id >= PWM_COUNT) return 0.0f;
  return desired_state_.pwm_values[id];
}

bool StateManager::get_desired_relay(RelayId id) const {
  if (id >= RELAY_COUNT) return false;
  return desired_state_.relay_values[id];
}

MaintenanceMode StateManager::get_maintenance_mode(RelayId id) const {
  if (id >= RELAY_COUNT) return MAINTENANCE_AUTO;
  return maint_modes_[id];
}

void StateManager::set_maintenance_mode(RelayId id, MaintenanceMode mode) {
  if (id >= RELAY_COUNT) return;
  if (mode == MAINTENANCE_FORCE_OFF) {
    desired_state_.relay_values[id] = false;
    this->update_effective_outputs();
  }
  maint_modes_[id] = mode;
  desired_state_.maint_modes[id] = mode;
  this->save_desired_state();
  ESP_LOGI(TAG, "Relay %d maintenance mode: %d", id, mode);
}

bool StateManager::check_interlocks(RelayId id, bool value) {
  if (!value) return true;
  switch (id) {
    case RELAY_CEILING:
      return (desired_state_.pwm_values[PWM_CEILING_WARM] > 0.01f || 
              desired_state_.pwm_values[PWM_CEILING_COLD] > 0.01f);
    case RELAY_12V:
      for (uint8_t i = PWM_DESK1_WARM; i < PWM_COUNT; i++) {
        if (desired_state_.pwm_values[i] > 0.01f) return true;
      }
      return false;
    case RELAY_STABILIZER:
      return (desired_state_.relay_values[RELAY_DESK1] ||
              desired_state_.relay_values[RELAY_DESK2] ||
              desired_state_.relay_values[RELAY_DESK3]);
    default:
      return true;
  }
}

void StateManager::update_dependency_relays() {
  bool ceiling_on = (desired_state_.pwm_values[PWM_CEILING_WARM] > 0.01f || 
                     desired_state_.pwm_values[PWM_CEILING_COLD] > 0.01f);
  if (maint_modes_[RELAY_CEILING] == MAINTENANCE_AUTO) {
    desired_state_.relay_values[RELAY_CEILING] = ceiling_on;
  }
  bool any_non_ceiling_pwm = false;
  for (uint8_t i = PWM_DESK1_WARM; i < PWM_COUNT; i++) {
    if (desired_state_.pwm_values[i] > 0.01f) {
      any_non_ceiling_pwm = true;
      break;
    }
  }
  if (maint_modes_[RELAY_12V] == MAINTENANCE_AUTO) {
    desired_state_.relay_values[RELAY_12V] = any_non_ceiling_pwm;
  }
  bool any_desk_on = (desired_state_.relay_values[RELAY_DESK1] ||
                      desired_state_.relay_values[RELAY_DESK2] ||
                      desired_state_.relay_values[RELAY_DESK3]);
  if (maint_modes_[RELAY_STABILIZER] == MAINTENANCE_AUTO) {
    desired_state_.relay_values[RELAY_STABILIZER] = any_desk_on;
  }
  this->save_desired_state();
  this->update_effective_outputs();
}
void StateManager::save_scene(uint8_t scene_id, const char* name) {
  if (scene_id >= MAX_SCENES) return;
  SceneData scene;
  scene.version = SCENE_STORAGE_VERSION;
  strncpy(scene.name, name, SCENE_NAME_MAX_LEN - 1);
  scene.name[SCENE_NAME_MAX_LEN - 1] = '\0';
  memcpy(scene.pwm_values, desired_state_.pwm_values, sizeof(desired_state_.pwm_values));
  memcpy(scene.relay_values, desired_state_.relay_values, sizeof(desired_state_.relay_values));
  scene.timestamp = (uint32_t) time(nullptr);
  scene.crc32 = this->calculate_crc32(&scene, sizeof(SceneData) - sizeof(uint32_t));
  char key[16];
  snprintf(key, sizeof(key), "scene_%d", scene_id);
  this->prefs_put(key, &scene, sizeof(SceneData));
  ESP_LOGI(TAG, "Saved scene %d: %s", scene_id, name);
}

bool StateManager::load_scene(uint8_t scene_id) {
  if (scene_id >= MAX_SCENES) return false;
  char key[16];
  snprintf(key, sizeof(key), "scene_%d", scene_id);
  SceneData scene;
  size_t len = this->prefs_get(key, &scene, sizeof(SceneData));
  if (len != sizeof(SceneData)) {
    ESP_LOGW(TAG, "Scene %d not found or corrupt", scene_id);
    return false;
  }
  uint32_t crc = this->calculate_crc32(&scene, sizeof(SceneData) - sizeof(uint32_t));
  if (crc != scene.crc32) {
    ESP_LOGE(TAG, "Scene %d CRC mismatch - corrupt!", scene_id);
    return false;
  }
  if (scene.version != SCENE_STORAGE_VERSION) {
    ESP_LOGW(TAG, "Scene %d version mismatch", scene_id);
    return false;
  }
  for (uint8_t i = 0; i < PWM_COUNT; i++) {
    this->set_desired_pwm(static_cast<PwmId>(i), scene.pwm_values[i]);
  }
  for (uint8_t i = 0; i < RELAY_COUNT; i++) {
    this->set_desired_relay(static_cast<RelayId>(i), scene.relay_values[i]);
  }
  ESP_LOGI(TAG, "Loaded scene %d: %s", scene_id, scene.name);
  return true;
}

void StateManager::delete_scene(uint8_t scene_id) {
  if (scene_id >= MAX_SCENES) return;
  char key[16];
  snprintf(key, sizeof(key), "scene_%d", scene_id);
  this->prefs_remove(key);
  ESP_LOGI(TAG, "Deleted scene %d", scene_id);
}

bool StateManager::restore_valid_state() {
  ESP_LOGI(TAG, "Attempting safe state restoration...");
  if (!this->load_desired_state()) {
    ESP_LOGW(TAG, "No valid desired state found, keeping outputs off");
    this->set_all_outputs_off();
    return false;
  }
  if (!this->validate_state_safety()) {
    ESP_LOGE(TAG, "Restored state unsafe - rejecting, outputs remain off");
    this->set_all_outputs_off();
    return false;
  }
  this->update_effective_outputs();
  ESP_LOGI(TAG, "Safe state restored successfully");
  return true;
bool StateManager::validate_state_safety() {
  for (uint8_t i = 0; i < RELAY_COUNT; i++) {
    if (maint_modes_[i] == MAINTENANCE_FORCE_OFF && desired_state_.relay_values[i]) {
      ESP_LOGW(TAG, "Relay %d FORCE_OFF conflicts with desired ON", i);
      return false;
    }
  }
  if (!this->check_interlocks(RELAY_CEILING, desired_state_.relay_values[RELAY_CEILING])) {
    ESP_LOGW(TAG, "Ceiling relay interlock violated");
    return false;
  }
  if (!this->check_interlocks(RELAY_12V, desired_state_.relay_values[RELAY_12V])) {
    ESP_LOGW(TAG, "12V relay interlock violated");
    return false;
  }
  if (!this->check_interlocks(RELAY_STABILIZER, desired_state_.relay_values[RELAY_STABILIZER])) {
    ESP_LOGW(TAG, "Stabilizer relay interlock violated");
    return false;
  }
  return true;
}

void StateManager::set_all_outputs_off() {
  for (uint8_t i = 0; i < PWM_COUNT; i++) {
    desired_state_.pwm_values[i] = 0.0f;
  }
  for (uint8_t i = 0; i < RELAY_COUNT; i++) {
    desired_state_.relay_values[i] = false;
  }
  this->update_effective_outputs();
}

void StateManager::update_effective_outputs() {
  this->update_dependency_relays();
  this->save_desired_state();
}

void StateManager::save_desired_state() {
  desired_state_.version = STATE_STORAGE_VERSION;
  desired_state_.timestamp = (uint32_t) time(nullptr);
  desired_state_.crc32 = this->calculate_crc32(&desired_state_, sizeof(DesiredState) - sizeof(uint32_t));
  this->prefs_put("desired_state", &desired_state_, sizeof(DesiredState));
}

bool StateManager::load_desired_state() {
  size_t len = this->prefs_get("desired_state", &desired_state_, sizeof(DesiredState));
  if (len != sizeof(DesiredState)) {
    memset(&desired_state_, 0, sizeof(DesiredState));
    for (uint8_t i = 0; i < RELAY_COUNT; i++) {
      maint_modes_[i] = MAINTENANCE_AUTO;
      desired_state_.maint_modes[i] = MAINTENANCE_AUTO;
    }
    return false;
  }
  uint32_t crc = this->calculate_crc32(&desired_state_, sizeof(DesiredState) - sizeof(uint32_t));
  if (crc != desired_state_.crc32) {
    ESP_LOGE(TAG, "Desired state CRC mismatch - corrupt!");
    memset(&desired_state_, 0, sizeof(DesiredState));
    return false;
  }
  if (desired_state_.version != STATE_STORAGE_VERSION) {
    ESP_LOGW(TAG, "Desired state version mismatch");
    return false;
  }
  for (uint8_t i = 0; i < RELAY_COUNT; i++) {
    maint_modes_[i] = desired_state_.maint_modes[i];
  }
  return true;
}

void StateManager::load_scenes() {}

uint32_t StateManager::calculate_crc32(const void* data, size_t length) {
  const uint8_t* bytes = static_cast<const uint8_t*>(data);
  uint32_t crc = 0xFFFFFFFF;
  for (size_t i = 0; i < length; i++) {
    crc ^= bytes[i];
    for (int j = 0; j < 8; j++) {
      crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));
    }
  }
  return ~crc;
}

void StateManager::restore_valid_state_service() {
  this->restore_valid_state();
}

void StateManager::save_scene_service(uint8_t scene_id, const std::string& name) {
  this->save_scene(scene_id, name.c_str());
}

void StateManager::load_scene_service(uint8_t scene_id) {
  this->load_scene(scene_id);
}

void StateManager::delete_scene_service(uint8_t scene_id) {
  this->delete_scene(scene_id);
}

std::string StateManager::get_state_summary() {
  char buf[512];
  int pos = 0;
  pos += snprintf(buf + pos, sizeof(buf) - pos, "Relays: ");
  for (uint8_t i = 0; i < RELAY_COUNT; i++) {
    pos += snprintf(buf + pos, sizeof(buf) - pos, "%d:%s ", i, 
                   desired_state_.relay_values[i] ? "ON" : "OFF");
  }
  pos += snprintf(buf + pos, sizeof(buf) - pos, "\nPWM: ");
  for (uint8_t i = 0; i < 6; i++) {
    pos += snprintf(buf + pos, sizeof(buf) - pos, "%d:%.0f%% ", i, 
                   desired_state_.pwm_values[i] * 100);
  }
  return std::string(buf);
}

}  // namespace automate
}  // namespace esphome
}