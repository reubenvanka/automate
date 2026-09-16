/*
  74HC595 + ULN2803 + 8-Channel Relay Test - Arduino Sketch
  For ESP32 dev board
  
  Hardware:
    74HC595 Shift Register:
      SER    -> GPIO0
      SRCLK  -> GPIO23
      RCLK   -> GPIO15
      OE (pin 13) -> GND (enable outputs)
      MR (pin 10) -> 3.3V (disable reset)
    
    ULN2803 Darlington Driver:
      In1-In8 <- 74HC595 Q0-Q7
      Out1-Out8 -> Relay Module IN1-IN8
      Pin 9 (GND) -> Common GND
      Pin 10 (COM) -> Relay VCC (5V/12V) - enables built-in flyback diodes
    
    Manual Override (Dip Switches):
      SW0-SW7 -> ULN2803 Out1-Out8 (wired-OR to GND)
      Other side of switches -> GND
      (No diodes needed - ULN2803 outputs are open-collector)

  Logic (Active-LOW Relay Modules):
    74HC595 bit = 1 -> ULN2803 input HIGH -> output LOW -> Relay ON
    74HC595 bit = 0 -> ULN2803 input LOW -> output HIGH-Z -> Relay OFF
    Manual Switch Closed -> ULN2803 output pulled to GND -> Relay ON
    Either ESP32 OR Manual Switch can turn relay ON

  Features:
    - ULN2803 provides 500mA/channel drive (vs 74HC595 6mA)
    - Built-in flyback diodes via COM pin
    - Manual override via dip switches on ULN2803 outputs
    - Robust serial command parsing
    - All relays OFF on startup
*/

const int DATA_PIN   = 0;   // SER - 74HC595 data input
const int CLOCK_PIN  = 23;  // SRCLK - 74HC595 shift register clock
const int LATCH_PIN  = 15;  // RCLK  - 74HC595 latch

// Relay states - each bit corresponds to Q0-Q7 (via ULN2803)
// With ULN2803 (inverting driver) + Active-LOW relay modules:
//   74HC595 bit = 1 -> ULN2803 input HIGH -> output LOW -> Relay ON
//   74HC595 bit = 0 -> ULN2803 input LOW -> output HIGH-Z -> Relay OFF
// Bit 0 = Q0/Relay 1, Bit 1 = Q1/Relay 2, ..., Bit 7 = Q7/Relay 8
uint8_t relay_state = 0x00;  // Start with all relays OFF (all bits 0 = HIGH-Z = OFF)

void setup() {
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);

  // Initialize all relays OFF (74HC595 outputs LOW = ULN2803 inputs LOW = HIGH-Z = OFF)
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, 0x00);
  digitalWrite(LATCH_PIN, HIGH);

  Serial.begin(115200);
  while (!Serial) delay(10);  // Wait for serial monitor

  Serial.println("74HC595 + ULN2803 Relay Test Started");
  Serial.println("Hardware: 74HC595 -> ULN2803 -> Relay Module (Active-LOW)");
  Serial.println("Manual Override: Dip switches on ULN2803 outputs to GND");
  Serial.println("Commands: Q0 ON, Q0 OFF, Q1 ON, Q1 OFF, ...");
  Serial.println("        ALL ON, ALL OFF, PATTERN1, PATTERN2, SCAN, HELP, DEBUG");
  delay(1000);

  // Ensure all relays are definitely OFF
  all_relays_off();
  delay(100);
}

void loop() {
  if (Serial.available() > 0) {
    // Read complete line (until newline)
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd == "HELP") {
      show_help();
    } else if (cmd == "DEBUG") {
      toggle_debug();
    } else if (cmd.startsWith("Q")) {
      // Parse Q0 ON, Q1 OFF, etc.
      // Expected format: Q<0-7> ON|OFF
      if (cmd.length() >= 3) {
        int relay_num = cmd[1] - '0';  // Get digit after Q
        String action = cmd.substring(2);

        if (relay_num >= 0 && relay_num <= 7) {
          if (action == "ON") {
            set_relay(relay_num, true);
            Serial.print("Q"); Serial.print(relay_num); Serial.println(" ON OK");
          } else if (action == "OFF") {
            set_relay(relay_num, false);
            Serial.print("Q"); Serial.print(relay_num); Serial.println(" OFF OK");
          } else {
            Serial.print("Unknown action for Q"); Serial.print(relay_num); 
            Serial.println(" - use ON or OFF");
          }
        } else {
          Serial.print("Relay number "); Serial.print(relay_num); 
          Serial.println(" - must be 0-7");
        }
      }
    } else if (cmd == "ALL ON") {
      all_relays_on();
      Serial.println("All relays ON");
    } else if (cmd == "ALL OFF") {
      all_relays_off();
      Serial.println("All relays OFF");
    } else if (cmd == "PATTERN1") {
      pattern1();
    } else if (cmd == "PATTERN2") {
      pattern2();
    } else if (cmd == "SCAN") {
      scan_relays();
    } else {
      Serial.println("Unknown command. Type HELP.");
    }
  }
}

void set_relay(int num, bool on) {
  // ULN2803 is inverting: input HIGH = output LOW = Relay ON (active-LOW)
  // 74HC595 bit = 1 -> ULN2803 input HIGH -> Relay ON
  // 74HC595 bit = 0 -> ULN2803 input LOW -> Relay OFF
  if (on) {
    relay_state |= (1 << num);   // Set bit = 1 = HIGH input = Relay ON
  } else {
    relay_state &= ~(1 << num);  // Clear bit = 0 = LOW input = Relay OFF
  }
  update_shift_register();
}

void all_relays_on() {
  relay_state = 0xFF;  // All bits 1 = all ULN2803 inputs HIGH = all relays ON
  update_shift_register();
}

void all_relays_off() {
  relay_state = 0x00;  // All bits 0 = all ULN2803 inputs LOW = all relays OFF
  update_shift_register();
}

void pattern1() {
  Serial.println("Pattern 1: Sequential scan");
  // Turn on Q0 through Q7 one by one
  for (int i = 0; i <= 7; i++) {
    set_relay(i, true);
    delay(200);
  }
  delay(500);
  // Turn off Q0 through Q7 one by one
  for (int i = 0; i <= 7; i++) {
    set_relay(i, false);
    delay(200);
  }
  delay(500);
}

void pattern2() {
  Serial.println("Pattern 2: Alternating");
  for (int i = 0; i < 4; i++) {
    // Odd relays ON (Q1, Q3, Q5, Q7), even OFF
    for (int j = 1; j <= 7; j += 2) {
      set_relay(j, true);
    }
    for (int j = 0; j <= 6; j += 2) {
      set_relay(j, false);
    }
    delay(400);

    // Even relays ON (Q0, Q2, Q4, Q6), odd OFF
    for (int j = 0; j <= 6; j += 2) {
      set_relay(j, true);
    }
    for (int j = 1; j <= 7; j += 2) {
      set_relay(j, false);
    }
    delay(400);
  }
}

void scan_relays() {
  Serial.println("Pattern 3: Full scan");
  all_relays_on();
  delay(800);
  all_relays_off();
  delay(800);
}

void update_shift_register() {
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, relay_state);
  digitalWrite(LATCH_PIN, HIGH);
}

void show_help() {
  Serial.println("Available commands:");
  Serial.println("  Q0 ON/Q0 OFF  - Relay Q0 on/off");
  Serial.println("  Q1 ON/Q1 OFF  - Relay Q1 on/off");
  Serial.println("  Q2 ON/Q2 OFF  - Relay Q2 on/off");
  Serial.println("  Q3 ON/Q3 OFF  - Relay Q3 on/off");
  Serial.println("  Q4 ON/Q4 OFF  - Relay Q4 on/off");
  Serial.println("  Q5 ON/Q5 OFF  - Relay Q5 on/off");
  Serial.println("  Q6 ON/Q6 OFF  - Relay Q6 on/off");
  Serial.println("  Q7 ON/Q7 OFF  - Relay Q7 on/off");
  Serial.println("  ALL ON        - All relays on");
  Serial.println("  ALL OFF       - All relays off");
  Serial.println("  PATTERN1      - Sequential scan pattern");
  Serial.println("  PATTERN2      - Alternating pattern");
  Serial.println("  SCAN          - All on then all off");
  Serial.println("  HELP          - Show this help");
  Serial.println("  DEBUG         - Toggle debug output");
}

bool debug_mode = false;
void toggle_debug() {
  debug_mode = !debug_mode;
  Serial.print("Debug mode: "); Serial.println(debug_mode ? "ON" : "OFF");
}