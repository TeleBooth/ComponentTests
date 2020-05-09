#include <communication.h>
#include <assert.h>

#include <error.h>

#if defined(KRPC_COMMUNICATION_POSIX)

//#include <fcntl.h>
//#include <stdint.h>
//#include <unistd.h>
#include <stdio.h>
#include <esp8266.h>

//extern int16_t responseSize;
//extern uint8_t UserTxBufferFS[2048];

krpc_error_t krpc_open(krpc_connection_t * connection, const krpc_connection_config_t * arg) {
  return KRPC_ERROR_NO_RESULTS;
}

krpc_error_t krpc_close(krpc_connection_t * connection) {
	//figure out what to do here
  return KRPC_OK;
}

int read (krpc_connection_t * connection, uint8_t * buf, size_t count){
	while(HAL_UART_Receive(connection->huart, buf, count, 2000) != HAL_OK);
	return connection->huart->RxXferSize - connection->huart->RxXferCount;
}

uint8_t readTotal = 0;
krpc_error_t krpc_read(krpc_connection_t * connection, uint8_t * buf, size_t count) {
	int total = 0;
	  	  while (true) {
	    	total += read(connection, buf+total, count-total);
	    	if (total == count){
	      		return KRPC_OK;
	    	}
			if (total == -1){
	          KRPC_RETURN_ERROR(IO, "read failed");
			}
		}
	      readTotal++;
	      return KRPC_OK;
}

krpc_error_t krpc_write(krpc_connection_t * connection, uint8_t * buf, size_t count) {
	HAL_StatusTypeDef result;
	while((result = HAL_UART_Transmit_DMA(connection->huart, buf, count)) == HAL_BUSY);
	HAL_Delay(10);
	if (result == HAL_ERROR)
		KRPC_RETURN_ERROR(IO, "write failed");
	return KRPC_OK;
}

#endif

#if defined(KRPC_COMMUNICATION_ARDUINO)

krpc_error_t krpc_open(krpc_connection_t * connection, const krpc_connection_config_t * arg) {
  if (arg)
    (*connection)->begin(arg->speed, arg->config);
  else
    (*connection)->begin(9600, SERIAL_8N1);
  while (!*connection) {
  }
  return KRPC_OK;
}

krpc_error_t krpc_close(krpc_connection_t * connection) {
  connection->end();
  return KRPC_OK;
}

krpc_error_t krpc_read(krpc_connection_t * connection, uint8_t * buf, size_t count) {
  size_t read = 0;
  while (true) {
    read += connection->readBytes(buf+read, count-read);
    if (read == count)
      return KRPC_OK;
  }
}

krpc_error_t krpc_write(krpc_connection_t * connection, const uint8_t * buf, size_t count) {
  if (count != connection->write(buf, count))
    KRPC_RETURN_ERROR(IO, "write failed");
  return KRPC_OK;
}

#endif  // KRPC_PLATFORM_ARDUINO
