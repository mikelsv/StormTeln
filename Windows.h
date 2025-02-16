#define MSVGUI_GLOBAL_SCALE 2.1f

class MsvWnd {
public:
	// Screen
	KiVec2 screen, pos;
	GLFWwindow *window;
	bool maximized, close_window;

	// Timer
	MTimer ttime;
	double stime;

	// FPS
	double fps_time;
	int fps, fps_count;

	// New
	MsvWnd() {
		screen = KiInt2(1024, 768);
		pos = KiInt2(-1, -1);

		fps_time = 0;
		close_window = 0;

		// Load config
		LoadConfig();
	}

	void Init(GLFWwindow *wnd) {
		window = wnd;

		// Timer
		ttime.Init();
		stime = ttime.dtime();
	}

	void UpdateFps() {
		fps_count ++;

		if (glfwGetTime() - fps_time >= 1.0) {
			fps_time += 1.0;

			fps = fps_count;
			fps_count = 0;
		}
	}

	// Set
	void SetScreen(int x, int y, bool m) {
		screen = KiInt2(x, y);
		maximized = m;
	}

	// Render
	void Render() {
		float wtime = (ttime.dtime() - stime) / 1000;
	}

	void LoadConfig() {
		MString data = LoadFile("StormTeln.cfg");
		XDataCont json(data);

		pos.x = json["screen.px"].toi();
		pos.y = json["screen.py"].toi();
		screen.x = json["screen.x"].toi();
		screen.y = json["screen.y"].toi();
		maximized = json["screen.max"].toi();

		if (screen.IsNull()) {
			screen = KiInt2(1024, 768);
			pos.x = -1;
		}
	}

	void SaveConfig() {
		KiInt2 pos;
		glfwGetWindowPos(window, &pos.x, &pos.y);
		int maximized = glfwGetWindowAttrib(window, GLFW_MAXIMIZED);

		JsonEncode json;

		json.Up("screen");
		json("x", itos(screen.x));
		json("y", itos(screen.y));
		json("px", itos(pos.x));
		json("py", itos(pos.y));
		json("max", itos(maximized));

		json.Save("StormTeln.cfg");
	}

	~MsvWnd() {	}

} MsvWnd;


static void MsvWndUpdate(GLFWwindow *window, float delta) {
	MsvWnd.UpdateFps();

	if (MsvWnd.close_window)
		glfwSetWindowShouldClose(window, 1);
}

static void MsvWndRenderScene(GLFWwindow *window, float delta) {
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	MsvWnd.Render();
}

static void MsvWndResize(GLFWwindow *window, int w, int h) {
	if (w < 1 || h < 1) {
		return;
	}

	if (MaticalsOpenGl.IsDisableResize())
		return;

	int maximized = glfwGetWindowAttrib(window, GLFW_MAXIMIZED);

	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	MsvWnd.SetScreen(w, h, maximized);
}

static void MsvWndKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	ImGuiIO &io = ImGui::GetIO();
	ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
}

static void MsvWndMouseClickCallback(GLFWwindow *window, int button, int action, int mods) {
	// ImGui
	ImGui::GetIO().MouseDown[button] = action;
}

static void MsvWndMouseMotionCallback(GLFWwindow *window, double x, double y) {
	ImGui::GetIO().MousePos = ImVec2(x, y);
}

void MsvWndMouseScrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
	ImGui::GetIO().MouseWheelH = xoffset;
	ImGui::GetIO().MouseWheel = yoffset;

	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
		return;
}

void MsvWndDrop(GLFWwindow *window, int count, const char **paths) {
	for (int i = 0; i < count; i++) {
		ILink link(paths[i]);
		VString file(paths[i]);

		// File operations
	}
}

void MsvWndRender(){
	bool show_window;// , disabled = 0;

	UGLOCK(StormTeln);

	// Window
	ImGui::Begin("Telnet", &show_window);
	ImGui::SetWindowFontScale(MSVGUI_GLOBAL_SCALE);

	// Connect
	if (StormTeln.GetState() == STELN_STATE_NONE)
		if (ImGui::Button("Connect")) {
			// Event: Button was clicked
			steln_url.CheckStrSize();
			StormTeln.DoConnect();
		}

	if (StormTeln.GetState() == STELN_STATE_CONNECTED)
		if (ImGui::Button("Disconnect")) {
			StormTeln.DoDisconnect();
		}

	if (StormTeln.GetState() == STELN_STATE_LISTEN)
		if (ImGui::Button("Stop")) {
			StormTeln.DoDisconnect();
		}

	ImGui::SameLine();

	// Url
	if (ImGui::InputText("##Url", steln_url, steln_url.GetMaxSize(), ImGuiInputTextFlags_EnterReturnsTrue)) {
		// Event: Enter key was pressed
		steln_url.CheckStrSize();
		StormTeln.DoConnect();
	}

	// Listen
	if (StormTeln.GetState() == STELN_STATE_NONE) {
		ImGui::SameLine();

		if (ImGui::Button("Listen")) {
			steln_url.CheckStrSize();
			StormTeln.DoListen();
		}
	}

	ImGui::Separator();

	editor.Render("TextEditor", ImVec2(-FLT_MIN, -75), 0);

	// Options
	ImGui::Checkbox("Echo", &steln_echo);
	ImGui::SameLine();
	ImGui::Checkbox("\\r", &steln_r);
	ImGui::SameLine();
	ImGui::Checkbox("\\n", &steln_n);

	// Send data Button //
	if (ImGui::Button("Send")) {
		StormTeln.DoSend();
	}

	ImGui::SameLine();

	// Send input
	if (ImGui::InputText("##Send", steln_input, steln_input.GetMaxSize(), ImGuiInputTextFlags_EnterReturnsTrue)) {
		StormTeln.DoSend();
	}

	ImGui::End();

	return;
}

void MsvWndClose(GLFWwindow *window) {
	MsvWnd.SaveConfig();
}

// Windows call
void MsvWndUpdateTitle() {
	glfwSetWindowTitle(MsvWnd.window, VString(LString() + StormTeln.GetTitle() + " - " + PROJECTNAME + " " + PROJECTVER[0].ver + _msv_zero_str));
}

void MsvWndClose() {
	MsvWnd.close_window = 1;
}




