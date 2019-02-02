#include "AmPmClient.h"

#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

// Based on Stimulant's Cinder Client example:
// https://github.com/stimulant/ampm/blob/master/samples/Cinder/Client/src/AMPMClient.cpp

namespace ampm {

	AmPmClient::AmPmClient(Options options) :
		mOptions(options),
		mSender(10000, "127.0.0.1", options.sendPort),
		mReceiver(options.receivePort)
	{
		// setup osc
		mSender.bind();
		mReceiver.bind();
		mReceiver.listen(nullptr);

		if (mOptions.autoUpdate) {
			startAutoUpdate();
		}

		if (mOptions.attachLogger) {
			mLogger = make_shared<AmPmLogger>(this);
			log::manager()->addLogger(mLogger);
		}
	}

	AmPmClient::~AmPmClient() {
		if (mLogger) {
			log::manager()->removeLogger(mLogger);
		}
	}

	void AmPmClient::update() {
		if (mOptions.framesPerHeartbeat <= 1 || --mFramesUntilHeartbeat <= 0) {
			sendHeartbeat();
			mFramesUntilHeartbeat = mOptions.framesPerHeartbeat;
		}
	}

	void AmPmClient::startAutoUpdate() {
		mUpdateConnection = App::get()->getSignalUpdate().connect(std::bind(&AmPmClient::update, this));
		mOptions.autoUpdate = true;
	}

	void AmPmClient::stopAutoUpdate() {
		mUpdateConnection.disconnect();
		mOptions.autoUpdate = false;
	}

	ci::JsonTree AmPmClient::getConfig(std::string host, int port) {
		JsonTree config;
		try {
			config = JsonTree(loadUrl(Url("http://" + host + ":" + toString(port) + "/config")));
		} catch (Exception error) {
			CI_LOG_EXCEPTION("Could not load config", error);
		}
		return config;
	}

	void AmPmClient::startReceiving() {
		mReceiver.listen([this](asio::error_code error, asio::ip::udp::endpoint endpoint) -> bool {
			if (error) {
				CI_LOG_E("Error Listening: " << error.message() << " val: " << error.value() << " endpoint: " << endpoint);
				App::get()->dispatchAsync([this] {
					mReceiver.close();
					mReceiver.bind();
					startReceiving();
				});
				return false;
			} else {
				return true;
			}
		});
	}

	void AmPmClient::sendHeartbeat() {
		osc::Message message;
		message.setAddress("/heart");
		mSender.send(message);
	}

	void AmPmClient::log(const log::Metadata & meta, const std::string & text) {
		osc::Message message;
		message.setAddress("/log");

		JsonTree arguments;
		arguments.pushBack(JsonTree("level", logLevelToString(meta.mLevel)));
		arguments.pushBack(JsonTree("message", text));
		arguments.pushBack(JsonTree("line", meta.mLocation.getFileName() + " (" + meta.mLocation.getFunctionName() + ")"));
		arguments.pushBack(JsonTree("lineNum", meta.mLocation.getLineNumber()));
		message.append(arguments.serialize());

		mSender.send(message);
	}

	void AmPmClient::sendAnalyticsEvent(const std::string & category, const std::string & action, const std::string & label, int value) {
		osc::Message message;
		message.setAddress("/event");

		JsonTree arguments;
		arguments.pushBack(JsonTree("Category", category));
		arguments.pushBack(JsonTree("Action", action));
		arguments.pushBack(JsonTree("Label", label));
		arguments.pushBack(JsonTree("Value", value));
		message.append(arguments.serialize());
		mSender.send(message);
	}

	void AmPmClient::sendCustomMessage(const std::string & address, const ci::JsonTree & msg) {
		osc::Message message;
		message.setAddress(address);
		message.append(msg.serialize());
		mSender.send(message);
	}

	std::string AmPmClient::logLevelToString(ci::log::Level level) {
		switch (level) {
		case ci::log::Level::LEVEL_ERROR:
		case ci::log::Level::LEVEL_FATAL:
			return "error";
		case ci::log::Level::LEVEL_WARNING:
			return "warn";
		case ci::log::Level::LEVEL_DEBUG:
		case ci::log::Level::LEVEL_INFO:
		case ci::log::Level::LEVEL_VERBOSE:
		default:
			return "info";
		}
	}


}

