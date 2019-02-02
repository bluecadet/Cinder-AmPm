#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/Log.h"
#include "cinder/Json.h"
#include "cinder/osc/Osc.h"

#include "AmPmLogger.h"

// Based on Stimulant's Cinder Client example:
// https://github.com/stimulant/ampm/blob/master/samples/Cinder/Client/include/AMPMClient.h

namespace ampm {

	typedef std::shared_ptr<class AmPmClient> AmPmClientRef;

	class AmPmClient {

	public:

		struct Options {
			int sendPort = 3002;		// Out UDP port
			int receivePort = 3003;		// In UDP port
			bool autoUpdate = true;		// Auto attach to update loop, so you don't have to call update()
			int framesPerHeartbeat = 1;	// How many frames per heart beat. This will affect am/pm's reported FPS.
			bool attachLogger = true;	// Attach AmPm logger to CI_LOG
		};

		AmPmClient(Options options = Options());
		virtual ~AmPmClient();

		void update();			// Only call if autoUpdate is false.
		void startAutoUpdate();	// Sets autoUpdate to true.
		void stopAutoUpdate();	// Sets autoUpdate to false.

		// Synchronously gets the current config from AM/PM. Will return an empty JSON on failure.
		ci::JsonTree getConfig(std::string host = "localhost", int port = 8888);

		// Explicitly log to AM/PM. Used by AmPmLogger when attachLogger is true.
		void log(const ci::log::Metadata & meta, const std::string & text);
		
		// Sends a Google Analytics event to AM/PM
		void sendAnalyticsEvent(const std::string & category = "", const std::string &  action = "", const std::string &  label = "", int value = 0);

		// Sends a custom JSON message to AM/PM
		void sendCustomMessage(const std::string & address, const ci::JsonTree & msg);

		// The current client options object.
		const Options & getOptions() { return mOptions; }

	protected:
		void sendHeartbeat();
		void startReceiving();
		std::string logLevelToString(ci::log::Level level);

		ci::osc::SenderUdp mSender;
		ci::osc::ReceiverUdp mReceiver;

		Options mOptions;
		ci::signals::ScopedConnection mUpdateConnection;
		AmPmLoggerRef mLogger = nullptr;

		int mFramesUntilHeartbeat = 0;

	};
}
