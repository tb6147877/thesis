#include "../NCLGL/window.h"
#include "Renderer.h"

int main()	{
	Window w("NCL Thesis!", 1280, 720, false);

	if(!w.HasInitialised()) {
		return -1;
	}
	
	Renderer renderer(w);
	if(!renderer.HasInitialised()) {
		return -1;
	}

	while(w.UpdateWindow()  && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)){
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_0)) {
			renderer.SwitchShadingType(Renderer::ShadingType::Forward);
		}
		else if (Window::GetKeyboard()->KeyDown(KEYBOARD_1)){
			renderer.SwitchShadingType(Renderer::ShadingType::Deferred);
		}
		else if (Window::GetKeyboard()->KeyDown(KEYBOARD_2)) {
			renderer.SwitchShadingType(Renderer::ShadingType::ForwardPlus);
		}
		else if (Window::GetKeyboard()->KeyDown(KEYBOARD_3)) {
			renderer.SwitchShadingType(Renderer::ShadingType::ForwardPlus_Debug_Lights);
		}
		else if (Window::GetKeyboard()->KeyDown(KEYBOARD_4)) {
			renderer.SwitchShadingType(Renderer::ShadingType::ForwardPlus_Debug_Depth);
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_8)) {
			renderer.SetShowSlice(true);
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_9)) {
			renderer.SetShowSlice(false);
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_P)) {
			renderer.SetExposure(0.01f);
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_L)) {
			renderer.SetExposure(-0.01f);
		}

		renderer.UpdateScene(w.GetTimer()->GetTimeDeltaSeconds());
		renderer.RenderScene();
		renderer.SwapBuffers();
		//std::cout << renderer.GetFPS() << std::endl;
		w.SetTitle("FPS:"+std::to_string(renderer.GetFPS()));
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5)) {
			Shader::ReloadAllShaders();
		}
	}
	return 0;
}