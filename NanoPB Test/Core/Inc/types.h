/*
 * types.h
 *
 *  Created on: May 15, 2020
 *      Author: Jeremy
 */

#ifndef INC_TYPES_H_
#define INC_TYPES_H_

//#include "decoders.h"
//#include "encoders.h"

typedef uint64_t krpc_object_t;

/**
 * Represents a reference frame for positions, rotations and
 * velocities. Contains:
 *
 * - The position of the origin.
 * - The directions of the x, y and z axes.
 * - The linear velocity of the frame.
 * - The angular velocity of the frame.
 *
 * This class does not contain any properties or methods. It is only
 * used as a parameter to other functions.
 */
typedef krpc_object_t krpc_SpaceCenter_ReferenceFrame_t;


/**
 * These objects are used to interact with vessels in KSP. This includes getting
 * orbital and flight data, manipulating control inputs and managing resources.
 * Created using SpaceCenter::active_vessel or SpaceCenter::vessels.
 */
typedef krpc_object_t krpc_SpaceCenter_Vessel_t;


// Contexts for encoding and decoding
typedef struct {
	uint8_t *message_string;
	// include other fields as I need them
	uint8_t message_len;
} string_context_t;

typedef struct {
	krpc_schema_ProcedureCall *calls;
	// include other fields as I need them
	uint8_t numCalls;
} proccall_context_t;

typedef struct {
	krpc_schema_Argument ** args;
	// include other fields as I need them
	uint8_t numArgs;
} procarg_context_t;

typedef struct {
	uint32_t service_id;
	uint32_t procedure_id;
	bool hasArgs;
} procserv_context_t;

typedef struct {
	void *valueDest;
	pb_msgdesc_t *valueField;
} procval_context_t;

typedef struct {
	void **valueDests;
	krpc_schema_ProcedureResult *resultDests;
	pb_msgdesc_t **resultFields;
	uint32_t numResults;
} procresult_context_t;


// Used as dummy structures for encoding and decoding values
pb_msgdesc_t byteDesc = { .default_value = 0, .field_callback = 0,
		.field_count = 1, .submsg_info = 0, .field_info = 0 };

#define BYTES &byteDesc

pb_msgdesc_t stringDesc = { .default_value = 0,
		.field_callback = 0, .field_count = 1, .submsg_info = 0,
		.field_info = 0 };

#define STRING &stringDesc

pb_msgdesc_t enumDesc = { .default_value = 0, .field_callback = 0,
		.field_count = 1, .submsg_info = 0, .field_info = 0 };

#define ENUM &enumDesc

pb_msgdesc_t objectDesc = { .default_value = 0, .field_callback = 0,
		.field_count = 1, .submsg_info = 0, .field_info = 0 };

#define OBJECT &enumDesc


#endif /* INC_TYPES_H_ */
