import os

path = '/Users/reubenvanka/Documents/iot-workspace/products/automate/firmware/esphome/packages/state_manager.h'

content = '''#pragma once

#include "esphome/core/component.h"
#include "esphome/core/preferences.h"
#include <vector>
#include <cstdint>
#include <cstring>

namespace esphome {
namespace automate {

enum RelayId : uint8_t {
  RELAY_FAN = 0,
  RELAY_CEILING = 1,
  RELAY_12V = 2,
  RELAY_STABILIZER = 3,
  RELAY_DESK1 = 4,
  RELAY_DESK2 = 5,
  RELAY_DESK3 = 6,
  RELAY_AC = 7,
  RELAY_30A = 8,
  RELAY_COUNT = 9
};

enum PwmId : uint8_t {
  PWM_CEILING_WARM = 0,
  PWM_CEILING_COLD = 1,
  PWM_DESK1_WARM = 2,
  PWM_DESK1_COLD = 3,
  PWM_DESK2_WARM = 4,
  PWM_DESK2_COLD = 5,
  PWM_MONITOR1_BACKLIGHT = 6,
  PWM_DESK_LIGHT1 = 7,
  PWM_TABLE_LIGHT = 8,
  PWM_SHELF_LIGHT1 = 9,
  PWM_MONITOR2_BACKLIGHT = 10,
  PWM_DESK_LIGHT2 = 11,
  PWM_SHELF_LIGHT2 = 12,
  PWM_WALL_BOUNCE = 13,
  PWM_PHOTO_SPOT = 14,
  PWM_CUPBOARD_BOUNCE = 15,
  PWM_CUPBOARD_SPOT = 16,
  PWM_DOOR_SPOT = 17,
  PWM_MONITOR_LIGHT3 = 18,
  PWM_MONITOR_LIGHT4 = 19,
  PWM_EXHAUST_FANS = 20,
  PWM_WASHROOM_LIGHT = 21,
  PWM_TERRACE_LIGHT = 22,
  PWM_TERRACE_AMBIENT = 23,
  PWM_COUNT = 24
};

enum MaintenanceMode : uint8_t {
  MAINTENANCE_AUTO = 0,
  MAINTENANCE_FORCE_ON = 1,
  MAINTENANCE_FORCE_OFF = 2
};

constexpr size_t MAX_SCENES = 10;
constexpr size_t SCENE_NAME_MAX_LEN = 32;
constexpr uint32_t SCENE_STORAGE_VERSION = 1;
constexpr uint32_t STATE_STORAGE_VERSION = 1;

struct SceneData {
  uint32_t version;
  uint32_t crc32;
  char name[SCENE_NAME_MAX_LEN];
  float pwm_values[PWM_COUNT];
  bool relay_values[RELAY_COUNT];
  uint32_t timestamp;
};

struct DesiredState {
  uint32_t version;
  uint32_t crc32;
  float pwm_values[PWM_COUNT];
  bool relay_values[RELAY_COUNT];
  MaintenanceMode maint_modes[RELAY_COUNT];
  uint32_t timestamp;
};

class StateManager : public Component, public PreferenceObject {
 public:
  StateManager() = default;

  void setup() override;
  void dump_config() override;

  void set_desired_pwm(PwmId id, float value);
  void set_desired_relay(RelayId id, bool value);
  float get_desired_pwm(PwmId id) const;
  bool get_desired_relay(RelayId id) const;
  MaintenanceMode get_maintenance_mode(RelayId id) const;
  void set_maintenance_mode(RelayId id, MaintenanceMode mode);

  bool check_interlocks(RelayId id, bool value);
  void update_dependency_relays();

  void save_scene(uint8_t scene_id, const char* name);
  bool load_scene(uint8_t scene_id);
  void delete_scene(uint8_t scene_id);

  bool restore_valid_state();
  bool validate_state_safety();
  void set_all_outputs_off();
  void update_effective_outputs();

  void save_desired_state();
  bool load_desired_state();
  void load_scenes();

  uint32_t calculate_crc32(const void* data, size_t length);

  void restore_valid_state_service();
  void save_scene_service(uint8_t scene_id, const std::string& name);
  void load_scene_service(uint8_t scene_id);
  void delete_scene_service(uint8_t scene_id);

  std::string get_state_summary();

 private:
  DesiredState desired_state_;
  MaintenanceMode maint_modes_[RELAY_COUNT];
  bool initialized_ = false;
};

}  // namespace automate
}  // namespace esphome
'''

with open(path, 'w') as f:
    f.write(content)
print('Written:', path)