#include <esp_now.h>
#include <WiFi.h>

// REPLACE WITH THE MAC Address of your receiver
uint8_t broadcastAddress[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

//Structure example to send data
//Must match the receiver structure
typedef struct struct_message {
  int value1;
  int value2;
  int value3;
} struct_message;

// Create a struct_message to hold the data for sending
struct_message sendData;

// Create a struct_message to hold incoming data
struct_message receiveData;

// Create peer interface
esp_now_peer_info_t peerInfo;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// Callback when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&receiveData, incomingData, sizeof(receiveData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.println("Data Received -");
  Serial.println(receiveData.value1);
  Serial.println(receiveData.value2);
  Serial.println(receiveData.value3);
  Serial.println();
}

void setup() {
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // obtain the mac address
  //Serial.println(WiFi.macAddress());
  String mac_addr = WiFi.macAddress();
  for (int i = 0; i < 6; i++) {
    String x = mac_addr.substring((i * 3), (i * 3) + 2);
    Serial.print("0x");
    Serial.print(x);
    if (i != 5) Serial.print(", ");
  }
  Serial.println();
  Serial.println("Update this mac address in broadcastAddress of other board, and re-uplod the code. Ignore if already done.");

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  // Register for a callback function that will be called when data is send
  esp_now_register_send_cb(OnDataSent);

  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // Set values to send
  sendData.value1 = random(0, 100);
  sendData.value2 = random(0, 100);
  sendData.value3 = random(0, 100);

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&sendData, sizeof(sendData));

  // show the results
  if (result == ESP_OK)
    Serial.println("Data sent successfully");
  else
    Serial.println("Error sending the data");

  delay(1000);
}
