#include <communication.h>

#include <error.h>

#if defined(KRPC_COMMUNICATION_POSIX)

//#include <fcntl.h>
//#include <stdint.h>
//#include <unistd.h>
#include <stdio.h>
#include <esp8266.h>

krpc_error_t krpc_open(krpc_connection_t * connection, const krpc_connection_config_t * arg) {
  return KRPC_ERROR_NO_RESULTS;
}

krpc_error_t krpc_close(krpc_connection_t * connection) {
	//figure out what to do here
  return KRPC_OK;
}

krpc_error_t krpc_read(krpc_connection_t * connection, uint8_t * buf, size_t count) {

    HAL_StatusTypeDef result = HAL_UART_Receive(connection->huart,  connection->inBufPtr , count, 2000);
    if (result == HAL_ERROR || result == HAL_BUSY) {
      KRPC_RETURN_ERROR(IO, "read failed");
    } else if (result == HAL_OK) {
      KRPC_RETURN_ERROR(EOF, "eof received");
    }
  return KRPC_OK;
}

krpc_error_t krpc_write(krpc_connection_t * connection, uint8_t * buf, size_t count) {
	HAL_StatusTypeDef result = HAL_UART_Transmit_DMA(connection->huart, buf, count);
	if (result == HAL_ERROR || result == HAL_BUSY)
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

krpc_error_t krpc_close(krpc_connection_t connection) {
  connection->end();
  return KRPC_OK;
}

krpc_error_t krpc_read(krpc_connection_t connection, uint8_t * buf, size_t count) {
  size_t read = 0;
  while (true) {
    read += connection->readBytes(buf+read, count-read);
    if (read == count)
      return KRPC_OK;
  }
}

krpc_error_t krpc_write(krpc_connection_t connection, const uint8_t * buf, size_t count) {
  if (count != connection->write(buf, count))
    KRPC_RETURN_ERROR(IO, "write failed");
  return KRPC_OK;
}

#endif  // KRPC_PLATFORM_ARDUINO
