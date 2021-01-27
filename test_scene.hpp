#pragma once

#include "scene.hpp"

#include "glm/glm.hpp"

class TestScene : public Scene {
public:
	virtual bool Init(Context& context) override;
	virtual void Tick(Context& context) override;
	virtual void Cleanup(Context& context) override;
	virtual void DoUI(Context& context) override;

private:
	glm::vec4 lineColor;
};