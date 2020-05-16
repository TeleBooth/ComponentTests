/*
 * decoders.h
 *
 *  Created on: May 15, 2020
 *      Author: Jeremy
 */

#ifndef INC_DECODERS_H_
#define INC_DECODERS_H_

#include "pb_decode.h"
#include "krpc.pb.h"
#include "types.h"

bool decode_bytes(pb_istream_t *stream, const pb_field_t *field, void **arg);
bool decode_ints(pb_istream_t *stream, const pb_field_t *field, void **arg);
bool decode_values(pb_istream_t *stream, const pb_field_t *field, void **arg);
bool decode_string(pb_istream_t *stream, const pb_field_t *field, void **arg);

// the huart structure
extern UART_HandleTypeDef huart2;

// decoding for specific fields
bool decode_string(pb_istream_t *stream, const pb_field_t *field, void **arg) {
	int i;
	int str_len;
	char *str = (char*) *arg;
	//discard the length byte
	pb_read(stream, str, 1);
	str_len = stream->bytes_left;
	// read until we're done, each field is preceded by it's length
	for (i = 0; i < str_len; i++) {
		if (!pb_read(stream, str + i, 1)) {
			return false;
		}
	}
	str[i] = '\0';
	return true;
}

bool decode_bytes(pb_istream_t *stream, const pb_field_t *field, void **arg) {
	int i;
	int byte_len;
	pb_byte_t *bytes = (pb_byte_t*) *arg;
	//discard the length byte
	//pb_read(stream, bytes, 1);
	byte_len = stream->bytes_left;
	// read until we're done
	for (i = 0; i < byte_len; i++) {
		if (!pb_read(stream, bytes + i, 1)) {
			return false;
		}
	}
	return true;
}

bool decode_results(pb_istream_t *stream, const pb_field_t *field, void **arg) {
	procresult_context_t *results = (procresult_context_t*) *arg;
	procval_context_t value;
	int i;
	bool result = false;
	for (i = 0; i < results->numResults; i++) {
		value.valueDest = results->valueDests[i];
		value.valueField = results->resultFields[i];
		results->resultDests[i].value.funcs.decode = &decode_values;
		results->resultDests[i].value.arg = &value;
		result = pb_decode(stream, krpc_schema_ProcedureResult_fields,
				&results->resultDests[i]);
		if (!result)
			return false;
	}
	return true;
}

bool decode_values(pb_istream_t *stream, const pb_field_t *field, void **arg) {
	procval_context_t *value = (procval_context_t*) *arg;
	// we could decode bytes, strings, or really whatever here
	if (value->valueField == BYTES ||
			value->valueField == ENUM) {
		return decode_bytes(stream, field, &value->valueDest);
	}
	if (value->valueField == STRING) {
		return decode_string(stream, field, &value->valueDest);
	}
	// if it's a submessage, like a Status response, then we need to decode it this way instead
	return pb_decode(stream, value->valueField, value->valueDest);
}

bool decode_error(pb_istream_t *stream, const pb_field_t *field,
		void *const*arg) {
	return false;
}



#endif /* INC_DECODERS_H_ */
