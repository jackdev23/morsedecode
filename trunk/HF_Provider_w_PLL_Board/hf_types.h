/*
 * hf_types.h
 *
 *  Created on: Mar 13, 2010
 *      Author: maelliot
 */

#ifndef HF_TYPES_H_
#define HF_TYPES_H_

typedef enum {
	HF_MSG_REPLY_ACK,
	HF_MSG_REPLY_ILL,
	HF_MSG_REPLY_SUPPRESS
} hf_message_response_t;

// hf link message hander functor type, returns 0 on success, otherwise failure
// the input data parameter is guaranteed to be "valid" only in the sense the
// messaging scheme validated it.  Also, only one data value is provided because
// that's all the messaging scheme provisions for.
typedef hf_message_response_t (*hf_link_msg_handler_t)(uint8_t);

#endif /* HF_TYPES_H_ */
