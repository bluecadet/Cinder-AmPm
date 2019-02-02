#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/Log.h"

#include "AmPmClient.h"

namespace ampm {

	class AmPmClient;

	typedef std::shared_ptr<class AmPmLogger> AmPmLoggerRef;

	class AmPmLogger : public ci::log::LoggerConsole {
	public:
		AmPmLogger(AmPmClient * client);
		~AmPmLogger();

		void write(const ci::log::Metadata & meta, const std::string & text) override;
		AmPmClient * mClient = nullptr;
	};
}
