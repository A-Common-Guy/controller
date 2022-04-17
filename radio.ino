#define NRF24_CHANNEL          100            // 0 ... 125
#define NRF24_CRC_LENGTH         RF24_CRC_16  // RF24_CRC_DISABLED, RF24_CRC_8, RF24_CRC_16 for 16-bit
#define NRF24_DATA_RATE          RF24_1MBPS // RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
#define NRF24_DYNAMIC_PAYLOAD    1
#define NRF24_PAYLOAD_SIZE        64            // Max. 64 bytes.
#define NRF24_PA_LEVEL           RF24_PA_MIN  // RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX    
#define NRF24_RETRY_DELAY        5            // Delay bewteen retries, 1..15.  Multiples of 250µs.
#define NRF24_RETRY_COUNT       5            // Number of retries, 1..15.

#define PROTOCOL 0x01                         // 0x01 (byte), )



void nrf24_setup()
{
  radio.begin();
  radio.enableDynamicPayloads();
  radio.setAutoAck(true);
  radio.setPALevel(NRF24_PA_LEVEL);
  radio.setRetries(NRF24_RETRY_DELAY, NRF24_RETRY_COUNT);
  radio.enableAckPayload();
  radio.setDataRate(NRF24_DATA_RATE);
  radio.setChannel(NRF24_CHANNEL);
  radio.setCRCLength(NRF24_CRC_LENGTH);
  radio.setPayloadSize(NRF24_PAYLOAD_SIZE);
  radio.openWritingPipe(rf24_tx);
  radio.stopListening();
}

void compose_payload(int flag,int16_t* center,int16_t* deviation,int16_t* floor_,int16_t* ceil_) {
  payload.flag = flag;
  payload.dxbtn = !digitalRead(DX_RETRO);
  payload.sxbtn = !digitalRead(SX_RETRO);
  update_pots(center,deviation,floor_,ceil_);
}
