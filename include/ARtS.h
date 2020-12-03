#ifndef __A_RT_S_H__
#define __A_RT_S_H__
#include "ArBase.h"
#include "IArRtcEngine.h"
#include <stdint.h>

class ARtSEvent
{
public:
	ARtSEvent(void) {};
	virtual ~ARtSEvent(void) {};
	virtual void on_join_channel_success(const char *channel, int elapsed) {};
	virtual void on_connection_lost(const char *channel) {};
	virtual void on_rejoin_channel_success(const char *channel, int elapsed) {};
	virtual void on_warning(const char *channel, int warning, const char *msg) {};
	virtual void on_error(const char *channel, int error, const char *msg) {};
	virtual void on_user_joined(const char *channel, const char * uid, int elapsed) {};
	virtual void on_user_offline(const char *channel, const char * uid, int reason) {};
	virtual void on_user_stream_open(const char *channel, const char * uid, int elapsed) {};
	virtual void on_user_stream_closed(const char *channel, const char * uid, int reason) {};
	virtual void on_user_mute_audio(const char *channel, const char * uid, int muted) {};
	virtual void on_user_mute_video(const char *channel, const char * uid, int muted) {};
	virtual void on_key_frame_gen_req(const char *channel, const char * requesting_uid) {};
	virtual void on_audio_data(const char *channel, const char * uid, uint32_t sent_ts, uint16_t seqn, uint8_t codec, const void *data, size_t len) {};
	virtual void on_video_data(const char *channel, const char * uid, uint32_t sent_ts, uint8_t codec, int is_key_frame, const void *data, size_t len) {};
	virtual void on_rdt_availability_changed(const char *channel, int is_available) {};
	virtual void on_cmd(const char *channel, const char * uid, int cmd, const void *param, size_t param_len) {};
	virtual void on_data_from_rdt(const char *channel, const char * uid, const void *data, size_t data_len) {};
	virtual void on_dec_bitrate(const char *channel, uint32_t bps) {};
	virtual void on_inc_bitrate(const char *channel, uint32_t bps) {};

	virtual void on_rtc_stats(const AR::RtcStats& stats) {};
	virtual void on_local_video_stats(const AR::LocalVideoStats& stats) {};
	virtual void on_remote_video_stats(const AR::RemoteVideoStats& stats) {};
	virtual void on_local_audio_stats(const AR::LocalAudioStats& stats) {};
	virtual void on_remote_audio_stats(const AR::RemoteAudioStats& stats) {};
};

class ARtS{
public:
	virtual ~ARtS(void) {};

	virtual int init(const char *app_id, const char * uid, const ARtSEvent *event_handler, const char *sdk_log_dir) = 0;
	virtual int set_log_level(int nLevel) = 0;
	virtual int config_log(int per_logfile_size, int logfile_roll_count) = 0;
	virtual int	set_parameters(const char*parameters) = 0;
	virtual int join_channel(const char *channel, const void *token, size_t len) = 0;
	virtual int renew_token(const void *token, size_t len) = 0;
	virtual int mute_local_audio(int mute) = 0;
	virtual int mute_remote_audio(const char * remote_uid, int mute) = 0;
	virtual int mute_local_video(int mute) = 0;
	virtual int mute_remote_video(const char * remote_uid, int mute) = 0;
	virtual int request_video_key_frame(const char * remote_uid, uint8_t stream_id) = 0;
	virtual int send_audio_data( uint8_t codec, const void *data, size_t data_len) = 0;
	virtual int send_video_data(uint8_t codec, int is_key_frame, const void *data, size_t data_len) = 0;
	virtual int leave_channel() = 0;
	virtual int fini() = 0;

protected:
	ARtS(void) : arts_event_(NULL) {};

	ARtSEvent	* arts_event_;
};

AR_API void AR_CALL initARtSEngine();
AR_API void AR_CALL deinitARtSEngine();

AR_API ARtS* AR_CALL createARtSEngine();

#endif	// __A_RT_S_H__
