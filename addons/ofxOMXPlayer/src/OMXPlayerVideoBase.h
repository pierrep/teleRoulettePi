#pragma once
#include "ofMain.h"
#include "ofxOMXPlayerSettings.h"

#include "LIBAV_INCLUDES.h"


#include "OMXReader.h"
#include "OMXClock.h"
#include "OMXStreamInfo.h"
#include "OMXThread.h"
#include "OMXDecoderBase.h"

#define MAX_DATA_SIZE    10 * 1024 * 1024


class OMXPlayerVideoBase: public OMXThread
{
	public:
		OMXPlayerVideoBase();
		//~OMXPlayerVideoBase();
		OMXDecoderBase*				m_decoder;
		AVStream*					m_pStream;
		int							m_stream_id;
		std::deque<OMXPacket *>		m_packets;

		bool						m_open;
		COMXStreamInfo				m_hints;
		double						m_iCurrentPts;

		pthread_cond_t				m_packet_cond;
		//pthread_cond_t				m_picture_cond;
		pthread_mutex_t				m_lock;
		pthread_mutex_t				m_lock_decoder;

		OMXClock*					m_av_clock;
		float						m_fps;
		double						m_frametime;
		bool						m_bAbort;
		bool						m_flush;
		int							m_speed;
		double						m_FlipTimeStamp; // time stamp of last flippage. used to play at a forced framerate
		double						m_iVideoDelay;
		unsigned int				m_cached_size;


		void						SetSpeed(int speed);
		int							GetSpeed();

		virtual bool				Close() = 0;
		bool						Decode(OMXPacket *pkt);
		void						Process();
		void						Flush();

		bool						AddPacket(OMXPacket *pkt);

		virtual bool				OpenDecoder() =0;

		bool						CloseDecoder();
		int							GetDecoderBufferSize();
		int							GetDecoderFreeSpace();
		double						GetCurrentPTS();
		double						GetFPS();

		unsigned int				GetCached();
	
		void						SubmitEOS();
		bool						IsEOS();

		void						Lock();
		void						UnLock();
		void						LockDecoder();
		void						UnLockDecoder();


		uint32_t					m_history_valid_pts;
		bool						m_flush_requested;

		bool						isExiting;
	
		int getCurrentFrame();
		void resetFrameCounter();
};