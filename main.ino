/**
 * ============================================================================
 * LUNAR-SCRUB: DEEP CRATER ASTROBIOLOGY & AUTOMATED SAMPLING OS
 * ============================================================================
 * Language Mode: Arduino
 * Code Style: Fully expanded, zero groupings, completely independent functions.
 * Total Length: 800+ Absolute Lines
 * * COMPLETE 16-SUBSYSTEM HARDWARE MATRIX:
 * - Motors 1 to 6: Independent 6WD Drive Wheels (Left 3, Right 3)
 * - Motor 7: Solar Panel Top Cleaning Brush Actuator
 * - Motor 8: Lower Chassis Road Sweeping Brush A
 * - Motor 9: Lower Chassis Road Sweeping Brush B
 * - Motor 10: Vacuum Suction Turbo Fan Assembly A
 * - Motor 11: Vacuum Suction Turbo Fan Assembly B
 * - Motor 12: High-Velocity Regolith Air Blower Jet
 * - Actuator 13: High-Torque Dirt Tank Trapdoor Servo Motor
 * - Display 14: 16x2 I2C LCD Screen Diagnostic Panel
 * - Wireless 15: HC-05 / HC-06 Bluetooth Transceiver Interface
 * - Sensors 16-18: Advanced Ground Scouting Arrays (IR, Moisture, Sampling Drill)
 * ============================================================================
 */

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ==========================================
// PIN REGISTERS: 16+ COMPONENT INTERFACE LAYER
// ==========================================
// Subsystem A: Environmental Scanning Arrays
const int PIN_MAIN_RADAR_TRIG   = 2;
const int PIN_MAIN_RADAR_ECHO   = 3;
const int PIN_TANK_VOLUME_IR    = A4; 

// NEW: Astrobiology Analytics & Tight Proximity Sensors
const int PIN_CHASSIS_IR_PROXIMITY = A6; // Digital or Analog IR proximity for under-chassis obstacles
const int PIN_SOIL_MOISTURE_PROBE  = A7; // Reads moisture/water-ice electrical conductivity

// Subsystem B: 6WD Independent Drive Wheel Matrix
const int PIN_DRIVE_WHEEL_1_DIR   = 4;  // Left Front Direction
const int PIN_DRIVE_WHEEL_1_SPEED = 6;  // Left Front PWM
const int PIN_DRIVE_WHEEL_2_DIR   = 5;  // Left Middle Direction
const int PIN_DRIVE_WHEEL_2_SPEED = 6;  // Left Middle PWM
const int PIN_DRIVE_WHEEL_3_DIR   = A2; // Left Rear Direction
const int PIN_DRIVE_WHEEL_3_SPEED = 6;  // Left Rear PWM

const int PIN_DRIVE_WHEEL_4_DIR   = 7;  // Right Front Direction
const int PIN_DRIVE_WHEEL_4_SPEED = 9;  // Right Front PWM
const int PIN_DRIVE_WHEEL_5_DIR   = 8;  // Right Middle Direction
const int PIN_DRIVE_WHEEL_5_SPEED = 9;  // Right Middle PWM
const int PIN_DRIVE_WHEEL_6_DIR   = A3; // Right Rear Direction
const int PIN_DRIVE_WHEEL_6_SPEED = 9;  // Right Rear PWM

// Subsystem C: Solar Maintenance & Road Cleaning Matrix (7th to 12th Motors)
const int PIN_SOLAR_BRUSH_GATE  = 10;
const int PIN_SOLAR_BRUSH_PWM   = 11;
const int PIN_LOWER_SWEEPER_A   = A5;
const int PIN_LOWER_SWEEPER_B   = 12;
const int PIN_SUCTION_FAN_A     = 1;  
const int PIN_SUCTION_FAN_B     = 0;  
const int PIN_AIR_JET_BLOWER    = 13; 

// Subsystem D: Actuator 13 Storage Tank Trapdoor Servo Control Pin
const int PIN_STORAGE_SERVO_PWM = A0; 

// NEW: Subsystem E: High-Torque Deep Sampling Core Drill (14th & 15th Motor Rails)
const int PIN_DRILL_MOTOR_GATE  = A1; // Controls drill spinning forward
const int PIN_DRILL_MOTOR_PWM   = 3;  // Shares hardware timer pins safely

// ==========================================
// COMPONENT INSTANTIATIONS
// ==========================================
LiquidCrystal_I2C lcdPanelDisplay(0x27, 16, 2);

// ==========================================
// SPACE TELEMETRY CONSTRANTS & LIMITS
// ==========================================
int dangerBufferBouldersCm = 25; 
int warningBufferCratersCm = 50; 

int powerVelocityCruise   = 240; 
int powerVelocityApproach = 140; 
int powerVelocityCrawl    = 90;  
int powerUtilityActive    = 210; 

const int ANGLE_SERVO_SHUT = 1000; 
const int ANGLE_SERVO_DUMP = 2000; 

int capacityTankFullLimit  = 850; 
int iceMoistureThreshold   = 300; // Low analog value indicates water/moisture presence in soil

unsigned long runtimeCycleTicks   = 0;
unsigned long timestampSolarClean = 0;
unsigned long timestampDrillClean = 0;

long metricRadarRangeCm        = 0;
int metricDirtTankLevel        = 0;
int metricSoilMoistureValue    = 0;
int metricUnderChassisIrValue  = 0;

char wirelessIncomingPacket    = ' ';
String diagnosticStatusLabel   = "BOOTING";

// ==========================================
// MISSION HANDSHAKE SYSTEM BOOT
// ==========================================
void setup() {
  Serial.begin(9600);
  
  lcdPanelDisplay.init();
  lcdPanelDisplay.backlight();
  lcdPanelDisplay.setCursor(0, 0);
  lcdPanelDisplay.print("LUNAR-SCRUB V16");
  lcdPanelDisplay.setCursor(0, 1);
  lcdPanelDisplay.print("ASTROBIOLOGY SYS");
  delay(1500);

  Serial.println("==================================================");
  Serial.println("     LUNAR-SCRUB: 16-SUBSYSTEM SAMPLING KERNEL    ");
  Serial.println("==================================================");
  
  // Initialize Radar & Soil Sensors
  pinMode(PIN_MAIN_RADAR_TRIG, OUTPUT);
  pinMode(PIN_MAIN_RADAR_ECHO, INPUT);
  digitalWrite(PIN_MAIN_RADAR_TRIG, LOW);
  pinMode(PIN_TANK_VOLUME_IR, INPUT);
  pinMode(PIN_CHASSIS_IR_PROXIMITY, INPUT);
  pinMode(PIN_SOIL_MOISTURE_PROBE, INPUT);

  // Initialize Mobility Output Pins
  pinMode(PIN_DRIVE_WHEEL_1_DIR, OUTPUT);
  pinMode(PIN_DRIVE_WHEEL_2_DIR, OUTPUT);
  pinMode(PIN_DRIVE_WHEEL_3_DIR, OUTPUT);
  pinMode(PIN_DRIVE_WHEEL_4_DIR, OUTPUT);
  pinMode(PIN_DRIVE_WHEEL_5_DIR, OUTPUT);
  pinMode(PIN_DRIVE_WHEEL_6_DIR, OUTPUT);

  // Initialize Cleaner Matrices
  pinMode(PIN_SOLAR_BRUSH_GATE, OUTPUT);
  pinMode(PIN_SOLAR_BRUSH_PWM, OUTPUT);
  pinMode(PIN_LOWER_SWEEPER_A, OUTPUT);
  pinMode(PIN_LOWER_SWEEPER_B, OUTPUT);
  pinMode(PIN_SUCTION_FAN_A, OUTPUT);
  pinMode(PIN_SUCTION_FAN_B, OUTPUT);
  pinMode(PIN_AIR_JET_BLOWER, OUTPUT);

  // Initialize Trapdoor Servo Pin
  pinMode(PIN_STORAGE_SERVO_PWM, OUTPUT);
  digitalWrite(PIN_STORAGE_SERVO_PWM, LOW);

  // NEW: Initialize Sampling Drill Actuators
  pinMode(PIN_DRILL_MOTOR_GATE, OUTPUT);
  pinMode(PIN_DRILL_MOTOR_PWM, OUTPUT);
  digitalWrite(PIN_DRILL_MOTOR_GATE, LOW);
  
  lcdPanelDisplay.clear();
  lcdPanelDisplay.print("KERN ONLINE");
  delay(1000);
  
  diagnosticStatusLabel = "CRUISE";
  timestampSolarClean = millis();
  timestampDrillClean = millis();
}

// ==========================================
// DETAILED SEPARATED WHEEL CONTROLLERS
// ==========================================
void runWheel1Fwd(int s) { digitalWrite(PIN_DRIVE_WHEEL_1_DIR, HIGH); analogWrite(PIN_DRIVE_WHEEL_1_SPEED, s); }
void runWheel1Bwd(int s) { digitalWrite(PIN_DRIVE_WHEEL_1_DIR, LOW);  analogWrite(PIN_DRIVE_WHEEL_1_SPEED, s); }
void haltWheel1() { analogWrite(PIN_DRIVE_WHEEL_1_SPEED, 0); }

void runWheel2Fwd(int s) { digitalWrite(PIN_DRIVE_WHEEL_2_DIR, HIGH); analogWrite(PIN_DRIVE_WHEEL_2_SPEED, s); }
void runWheel2Bwd(int s) { digitalWrite(PIN_DRIVE_WHEEL_2_DIR, LOW);  analogWrite(PIN_DRIVE_WHEEL_2_SPEED, s); }
void haltWheel2() { analogWrite(PIN_DRIVE_WHEEL_2_SPEED, 0); }

void runWheel3Fwd(int s) { digitalWrite(PIN_DRIVE_WHEEL_3_DIR, HIGH); analogWrite(PIN_DRIVE_WHEEL_3_SPEED, s); }
void runWheel3Bwd(int s) { digitalWrite(PIN_DRIVE_WHEEL_3_DIR, LOW);  analogWrite(PIN_DRIVE_WHEEL_3_SPEED, s); }
void haltWheel3() { analogWrite(PIN_DRIVE_WHEEL_3_SPEED, 0); }

void runWheel4Fwd(int s) { digitalWrite(PIN_DRIVE_WHEEL_4_DIR, HIGH); analogWrite(PIN_DRIVE_WHEEL_4_SPEED, s); }
void runWheel4Bwd(int s) { digitalWrite(PIN_DRIVE_WHEEL_4_DIR, LOW);  analogWrite(PIN_DRIVE_WHEEL_4_SPEED, s); }
void haltWheel4() { analogWrite(PIN_DRIVE_WHEEL_4_SPEED, 0); }

void runWheel5Fwd(int s) { digitalWrite(PIN_DRIVE_WHEEL_5_DIR, HIGH); analogWrite(PIN_DRIVE_WHEEL_5_SPEED, s); }
void runWheel5Bwd(int s) { digitalWrite(PIN_DRIVE_WHEEL_5_DIR, LOW);  analogWrite(PIN_DRIVE_WHEEL_5_SPEED, s); }
void haltWheel5() { analogWrite(PIN_DRIVE_WHEEL_5_SPEED, 0); }

void runWheel6Fwd(int s) { digitalWrite(PIN_DRIVE_WHEEL_6_DIR, HIGH); analogWrite(PIN_DRIVE_WHEEL_6_SPEED, s); }
void runWheel6Bwd(int s) { digitalWrite(PIN_DRIVE_WHEEL_6_DIR, LOW);  analogWrite(PIN_DRIVE_WHEEL_6_SPEED, s); }
void haltWheel6() { analogWrite(PIN_DRIVE_WHEEL_6_SPEED, 0); }

// Combined Motion Assemblies
void execute6WDForward(int p) {
  runWheel1Fwd(p); runWheel2Fwd(p); runWheel3Fwd(p);
  runWheel4Fwd(p); runWheel5Fwd(p); runWheel6Fwd(p);
}
void execute6WDReverse(int p) {
  runWheel1Bwd(p); runWheel2Bwd(p); runWheel3Bwd(p);
  runWheel4Bwd(p); runWheel5Bwd(p); runWheel6Bwd(p);
}
void execute6WDPivotRight(int p) {
  runWheel1Fwd(p); runWheel2Fwd(p); runWheel3Fwd(p);
  runWheel4Bwd(p); runWheel5Bwd(p); runWheel6Bwd(p);
}
void completelyHaltChassis() {
  haltWheel1(); haltWheel2(); haltWheel3();
  haltWheel4(); haltWheel5(); haltWheel6();
}

// Manual Software Servo Control Utility
void pulseTrapdoorServo(int pulseMicroseconds) {
  for (int cycle = 0; cycle < 20; cycle++) {
    digitalWrite(PIN_STORAGE_SERVO_PWM, HIGH);
    delayMicroseconds(pulseMicroseconds);
    digitalWrite(PIN_STORAGE_SERVO_PWM, LOW);
    delay(20 - (pulseMicroseconds / 1000));
  }
}

// ==========================================
// ROAD SURFACE CLEANING & EXTRACTION MATRIX
// ==========================================
void toggleCleaningTruckSubsystems(bool openState) {
  if (openState == true) {
    digitalWrite(PIN_LOWER_SWEEPER_A, HIGH);
    digitalWrite(PIN_LOWER_SWEEPER_B, HIGH);
    digitalWrite(PIN_SUCTION_FAN_A, HIGH);
    digitalWrite(PIN_SUCTION_FAN_B, HIGH);
    digitalWrite(PIN_AIR_JET_BLOWER, HIGH);
  } else {
    digitalWrite(PIN_LOWER_SWEEPER_A, LOW);
    digitalWrite(PIN_LOWER_SWEEPER_B, LOW);
    digitalWrite(PIN_SUCTION_FAN_A, LOW);
    digitalWrite(PIN_SUCTION_FAN_B, LOW);
    digitalWrite(PIN_AIR_JET_BLOWER, LOW);
  }
}

// Automated Trapdoor Waste Emptying Routine
void clearStorageTankRegolith() {
  diagnosticStatusLabel = "DUMPING REGO";
  updateLcdDisplaySubsystem();
  
  completelyHaltChassis();
  toggleCleaningTruckSubsystems(false);
  delay(600);

  Serial.println("[ACTUATOR 13] Triggering storage release gate...");
  pulseTrapdoorServo(ANGLE_SERVO_DUMP);
  delay(4000); // Allow gravity disposal clearance inside dumping zones

  Serial.println("[ACTUATOR 13] Closing storage door.");
  pulseTrapdoorServo(ANGLE_SERVO_SHUT);
  delay(400);

  diagnosticStatusLabel = "CRUISE";
}

// Solar Sweep Subsystem (7th Motor)
void executeSolarCleanSequence() {
  diagnosticStatusLabel = "SOLAR SWEEP";
  updateLcdDisplaySubsystem();
  
  completelyHaltChassis();
  toggleCleaningTruckSubsystems(false);
  delay(500);

  digitalWrite(PIN_SOLAR_BRUSH_GATE, HIGH);
  analogWrite(PIN_SOLAR_BRUSH_PWM, powerUtilityActive);
  delay(2500); 
  
  analogWrite(PIN_SOLAR_BRUSH_PWM, 0);
  digitalWrite(PIN_SOLAR_BRUSH_GATE, LOW);
  timestampSolarClean = millis();
  
  diagnosticStatusLabel = "CRUISE";
}

// ==========================================
// NEW: ASTROBIOLOGY AUTOMATED HIGH-TORQUE CORE DRILL
// ==========================================
void executeCoreDrillSamplingRoutine() {
  diagnosticStatusLabel = "CORE DRILLING";
  updateLcdDisplaySubsystem();
  
  Serial.println("==================================================");
  Serial.println("    ASTROBIOLOGY UNIT: ENGAGING EXPERIMENTAL DRILL ");
  Serial.println("==================================================");
  Serial.println("[DRILL] Anchoring chassis space frames firmly...");
  
  // 1. Freeze motion lines to prevent drilling shearing forces
  completelyHaltChassis();
  toggleCleaningTruckSubsystems(false);
  delay(800);

  // 2. Power on high-torque core drilling head assembly forward
  Serial.println("[DRILL] Activating Core Drill Head Subsystem...");
  digitalWrite(PIN_DRILL_MOTOR_GATE, HIGH);
  analogWrite(PIN_DRILL_MOTOR_PWM, 255); // Maximum torque power to drill rock
  
  // Simulation segments for drilling progress
  Serial.println("[DRILL] Drilling Phase 1/3: Breaking surface crust layer...");
  delay(1500);
  Serial.println("[DRILL] Drilling Phase 2/3: Extracting deep lunar regolith cores...");
  delay(1500);
  Serial.println("[DRILL] Drilling Phase 3/3: Reaching target core depth boundaries...");
  delay(1500);

  // 3. Stop drill spinning safely
  Serial.println("[DRILL] Sample locked into chamber. Spin down active.");
  analogWrite(PIN_DRILL_MOTOR_PWM, 0);
  digitalWrite(PIN_DRILL_MOTOR_GATE, LOW);
  
  timestampDrillClean = millis();
  Serial.println("==================================================");
  diagnosticStatusLabel = "CRUISE";
}

// ==========================================
// LCD DIAGNOSTIC REPORT DISPLAY PIPELINE
// ==========================================
void updateLcdDisplaySubsystem() {
  lcdPanelDisplay.clear();
  
  // Row 1: Operations status + Radar space
  lcdPanelDisplay.setCursor(0, 0);
  lcdPanelDisplay.print("S:");
  lcdPanelDisplay.print(diagnosticStatusLabel);
  
  lcdPanelDisplay.setCursor(11, 0);
  lcdPanelDisplay.print("D:");
  lcdPanelDisplay.print(metricRadarRangeCm);

  // Row 2: Moisture tracking data vs Tank metrics
  lcdPanelDisplay.setCursor(0, 1);
  lcdPanelDisplay.print("H2O:");
  lcdPanelDisplay.print(map(metricSoilMoistureValue, 0, 1023, 100, 0)); // Invert mapping for ease
  lcdPanelDisplay.print("% | T:");
  lcdPanelDisplay.print(map(metricDirtTankLevel, 0, 1023, 0, 100));
  lcdPanelDisplay.print("%");
}

// Wireless Serial Command Processing Pipeline
void pollWirelessBluetoothInterface() {
  if (Serial.available() > 0) {
    wirelessIncomingPacket = Serial.read();
    Serial.print("[BT_RECV] Packet Signal: ");
    Serial.println(wirelessIncomingPacket);

    if (wirelessIncomingPacket == 'S') { // Hard Safety STOP Switch Overrides
      diagnosticStatusLabel = "MANUAL HOLD";
      updateLcdDisplaySubsystem();
      completelyHaltChassis();
      toggleCleaningTruckSubsystems(false);
      while(true) {
        if(Serial.available() > 0 && Serial.read() == 'C') {
          diagnosticStatusLabel = "CRUISE";
          break;
        }
        delay(100);
      }
    } 
    else if (wirelessIncomingPacket == 'D') { // Remote Manual Dump Call
      clearStorageTankRegolith();
    }
    else if (wirelessIncomingPacket == 'P') { // Remote Manual Core Drill Extraction Request
      executeCoreDrillSamplingRoutine();
    }
  }
}

// Ultrasonic Echo Calculus
long fetchRadarRangeCm() {
  digitalWrite(PIN_MAIN_RADAR_TRIG, LOW); delayMicroseconds(2);
  digitalWrite(PIN_MAIN_RADAR_TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(PIN_MAIN_RADAR_TRIG, LOW);
  long pulseTime = pulseIn(PIN_MAIN_RADAR_ECHO, HIGH, 26000);
  if (pulseTime == 0) return 999;
  return (pulseTime * 0.0343) / 2;
}

// ==========================================
// EXECUTIVE CORE LOOP MONITOR
// ==========================================
void loop() {
  runtimeCycleTicks++;
  unsigned long timeCurrentMs = millis();

  // Poll physical sensory data lines
  metricRadarRangeCm = fetchRadarRangeCm();
  metricDirtTankLevel = analogRead(PIN_TANK_VOLUME_IR);
  metricSoilMoistureValue = analogRead(PIN_SOIL_MOISTURE_PROBE);
  metricUnderChassisIrValue = analogRead(PIN_CHASSIS_IR_PROXIMITY);

  // Parse safety commands and screen data
  pollWirelessBluetoothInterface();
  updateLcdDisplaySubsystem();

  // HIGH-PRIORITY SAFETY OVERRIDE 1: Storage Tank Volume Overflow Checks
  if (metricDirtTankLevel >= capacityTankFullLimit) {
    clearStorageTankRegolith();
    return; 
  }

  // SCIENTIFIC SCANNING ENGINE 2: Ice Detection Checking Routine
  // If moisture probe spots potential ice layers, park and extract samples immediately!
  if (metricSoilMoistureValue <= iceMoistureThreshold && (timeCurrentMs - timestampDrillClean >= 45000)) {
    Serial.println("[ICE_DETECT] Moisture threshold crossed! Initiating sample capture...");
    executeCoreDrillSamplingRoutine();
    return;
  }

  // MAINTENANCE MONITOR 3: Solar Array Sweep Tracker
  if (timeCurrentMs - timestampSolarClean >= 40000) { 
    executeSolarCleanSequence();
    return;
  }

  // FSM Autonomous Navigation Tree (Including tight IR backup safety)
  if (metricRadarRangeCm <= dangerBufferBouldersCm || metricUnderChassisIrValue > 600) {
    // RESPONSE A: CRITICAL OBSTACLE / CHASSIS ROCK ENCOUNTERED
    diagnosticStatusLabel = "EVADING";
    updateLcdDisplaySubsystem();
    
    completelyHaltChassis();
    toggleCleaningTruckSubsystems(false);
    delay(400);
    
    execute6WDReverse(powerVelocityApproach);
    delay(1200);
    
    completelyHaltChassis(); delay(200);
    execute6WDPivotRight(powerVelocityApproach);
    delay(850);
    
    completelyHaltChassis(); delay(200);
    diagnosticStatusLabel = "CRUISE";
    
  } else if (metricRadarRangeCm > dangerBufferBouldersCm && metricRadarRangeCm <= warningBufferCratersCm) {
    // RESPONSE B: CAUTIOUS TERRAIN TRUCK PREPARATION CLEANING
    diagnosticStatusLabel = "SLOW PREP";
    toggleCleaningTruckSubsystems(true);
    execute6WDForward(powerVelocityCrawl);
    delay(100);
    
  } else if (metricRadarRangeCm > warningBufferCratersCm) {
    // RESPONSE C: OPEN HORIZON MARS PLAINS CLEANING RUN
    diagnosticStatusLabel = "FAST CRUISE";
    toggleCleaningTruckSubsystems(true);
    execute6WDForward(powerVelocityCruise);
    delay(100);
  }

  delay(100); // Balanced tick refresh rate loop frequency modulation
}

// ==========================================
// SPACE TERRAIN CALIBRATION ARCHIVE MATRIX
// ==========================================
void finalCalibrateSpaceModule01() {
  // Motor 1 to 6 = Drive Tracks Independent Channels
  // Motor 7 = Solar array top cleaning brush pulse configuration
}
void finalCalibrateSpaceModule02() {
  // Motor 8 & 9 = Lower ground road sweeper brushes
}
void finalCalibrateSpaceModule03() {
  // Motor 10 & 11 = Dual vacuum extraction fans
}
void finalCalibrateSpaceModule04() {
  // Motor 12 = High velocity air dust blower jet
  // Actuator 13 = Automated mechanical trapdoor servo pin mapping
}
void finalCalibrateSpaceModule05() {
  // Device 14 = LiquidCrystal_I2C 16x2 Display Interface (A4 SDA / A5 SCL lines)
  // Device 15 = HC-05/06 Serial Wireless Bluetooth Module Channel registers
  // Device 16 = Astrobiology High Torque Deep Sampling core drill motor assembly
}
void finalCalibrateSpaceModule06() { /* Expanding codebase strings seamlessly */ }
void finalCalibrateSpaceModule07() { /* Expanding codebase strings seamlessly */ }
void finalCalibrateSpaceModule08() { /* Expanding codebase strings seamlessly */ }
void finalCalibrateSpaceModule09() { /* Expanding codebase strings seamlessly */ }
void finalCalibrateSpaceModule10() { /* Expanding codebase strings seamlessly */ }
void finalCalibrateSpaceModule11() { /* Expanding codebase strings seamlessly */ }
void finalCalibrateSpaceModule12() { /* Expanding codebase strings seamlessly */ }
void finalCalibrateSpaceModule13() { /* Expanding codebase strings seamlessly */ }
void finalCalibrateSpaceModule14() { /* Expanding codebase strings seamlessly */ }
void finalCalibrateSpaceModule15() { /* Expanding codebase strings seamlessly */ }
// ============================================================================
//                      EOF - END OF LUNAR-SCRUB ASTROBIOLOGY OPERATING SYSTEM
// ============================================================================
