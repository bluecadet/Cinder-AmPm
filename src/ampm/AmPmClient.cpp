#include "AmPmClient.h"

#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

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
		//startReceiving();

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

	void AmPmClient::sendHeartbeat() {
		osc::Message message;
		message.setAddress("/heart");
		mSender.send(message);
	}

	void AmPmClient::update() {
		sendHeartbeat();
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

