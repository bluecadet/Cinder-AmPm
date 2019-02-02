#include "AmPmClient.h"

#include "cinder/Log.h"
#include "AmPmLogger.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace ampm {

	AmPmLogger::AmPmLogger(AmPmClient * client) :
		mClient(client)
	{
	}

	AmPmLogger::~AmPmLogger() {
	}
	void AmPmLogger::write(const ci::log::Metadata & meta, const std::string & text) {
		if (mClient) {
			mClient->log(meta, text);
		}
	}

}

