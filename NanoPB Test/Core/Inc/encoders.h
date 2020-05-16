/*
 * encoders.h
 *
 *  Created on: May 15, 2020
 *      Author: Jeremy
 */

#ifndef INC_ENCODERS_H_
#define INC_ENCODERS_H_

#include "pb_encode.h"
#include "krpc.pb.h"
#include "types.h"

bool encode_string(pb_ostream_t *stream, const pb_field_t *field,
		void *const*arg);

// the huart structure
extern UART_HandleTypeDef huart2;



bool encode_string(pb_ostream_t *stream, const pb_field_t *field,
		void *const*arg) {
	uint8_t *str = (uint8_t *) *arg;

	if (!pb_encode_tag_for_field(stream, field))
		return false;

	return pb_encode_string(stream, str, strlen((char*) str));
}

bool encode_uint64(pb_ostream_t *stream, const pb_field_t *field,
		void *const*arg) {
	uint64_t *numPtr = (uint64_t *) *arg;
	if (!pb_encode_tag(stream, PB_WT_VARINT, field->tag))
			return false;

	return pb_encode_fixed64(stream, numPtr);
}

bool encode_calls(pb_ostream_t *stream, const pb_field_t *field,
		void *const*arg) {
	proccall_context_t *calls = (proccall_context_t*) *arg;
	int i;
	bool result = false;
	for (i = 0; i < calls->numCalls; i++) {
		if (!pb_encode_tag_for_field(stream, field))
			return false;
		result = pb_encode_submessage(stream, krpc_schema_ProcedureCall_fields,
				&calls->calls[i]);
		if (!result)
			return false;
	}
	return true;
}

bool encode_args(pb_ostream_t *stream, const pb_field_t *field, void *const*arg) {
	procarg_context_t *procarg = (procarg_context_t*) *arg;
	int i;
	bool result = false;

	for (i = 0; i < procarg->numArgs; i++) {
		if (!pb_encode_tag_for_field(stream, field))
						return false;
		result = pb_encode_submessage(stream, krpc_schema_Argument_fields,
				procarg->args[i]);
		if (!result)
			return false;
	}
	return true;
}

#endif /* INC_ENCODERS_H_ */
