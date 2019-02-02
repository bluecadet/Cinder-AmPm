#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/Log.h"
#include "cinder/Json.h"
#include "cinder/osc/Osc.h"

#include "AmPmLogger.h"

namespace ampm {

	typedef std::shared_ptr<class AmPmClient> AmPmClientRef;

	class AmPmClient {

	public:

		struct Options {
			int sendPort = 3002;		// out UDP port
			int receivePort = 3003;		// in UDP port
			bool autoUpdate = true;		// auto attach to update loop
			int updateInterval = 1;		// how many frames per update
			bool attachLogger = true;	// Attach AmPm logger to CI_LOG
		};

		AmPmClient(Options options = Options());
		virtual ~AmPmClient();

		void sendHeartbeat();

		// Only call if autoUpdate is false
		void update();
		void startAutoUpdate();
		void stopAutoUpdate();

		ci::JsonTree getConfig(std::string host = "localhost", int port = 8888);
		const Options & getOptions() { return mOptions; }
		void log(const ci::log::Metadata & meta, const std::string & text);

	protected:
		void startReceiving();
		std::string logLevelToString(ci::log::Level level);

		ci::osc::SenderUdp mSender;
		ci::osc::ReceiverUdp mReceiver;

		Options mOptions;
		ci::signals::ScopedConnection mUpdateConnection;
		AmPmLoggerRef mLogger = nullptr;

	};
}
