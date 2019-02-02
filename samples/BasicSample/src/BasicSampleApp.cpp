#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "ampm/AmPmClient.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class BasicSampleApp : public App {
public:
	void setup() override;
	void mouseDown(MouseEvent event) override;
	void update() override;
	void draw() override;

protected:
	ampm::AmPmClientRef mClient = nullptr;
};

void BasicSampleApp::setup() {
	ampm::AmPmClient::Options options;
	mClient = make_shared<ampm::AmPmClient>(options);
	CI_LOG_D("Test debug log");
	CI_LOG_I("Test info log");
	CI_LOG_W("Test warning log");
	CI_LOG_E("Test error log");
}

void BasicSampleApp::mouseDown(MouseEvent event) {
}

void BasicSampleApp::update() {
}

void BasicSampleApp::draw() {
	gl::clear(Color(0, 0, 0));
}

CINDER_APP(BasicSampleApp, RendererGl)
