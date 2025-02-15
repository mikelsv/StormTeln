#define MSVGUI_GLOBAL_SCALE 2.2

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
	bool show_window, disabled = 0;

	// Window
	ImGui::Begin("Telnet", &show_window);
	ImGui::SetWindowFontScale(MSVGUI_GLOBAL_SCALE);

	if (StormTeln.GetState() != STELN_STATE_NONE && StormTeln.GetState() != STELN_STATE_CONNECTED) {
		// Disable the following widgets
		ImGui::BeginDisabled(true); // Pass 'true' to disable widgets
		disabled = 1;
	}

	// Button
	// ImGui::Button creates a clickable button
	if (ImGui::Button(StormTeln.GetState() == STELN_STATE_NONE ? "Connect" : "Disconnect")) {
		// Event: Button was clicked
		//inputText = buffer; // Save the entered text into the global variable
		//ImGui::Text("Button clicked! Text: %s", storm_url); // Display a message with the entered text
		steln_url.CheckStrSize();
		StormTeln.DoConnect();
	}

	if(disabled)
		ImGui::EndDisabled(); // Re-enable widgets

	ImGui::SameLine();

	// Input text field
	// ImGui::InputText creates a text input field where the user can type
	// The flag ImGuiInputTextFlags_EnterReturnsTrue makes the function return true when Enter is pressed
	if (ImGui::InputText("Url", steln_url, steln_url.GetMaxSize(), ImGuiInputTextFlags_EnterReturnsTrue)) {
		// Event: Enter key was pressed
		steln_url.CheckStrSize();
		StormTeln.DoConnect();

		//inputText = buffer; // Save the entered text into the global variable
		//ImGui::Text("Enter pressed! Text: %s", storm_url); // Display a message with the entered text
	}

	ImGui::Separator();

	// Create a buffer for the multi-line text
	//char buffer[1024];
	//strncpy(buffer, multiLineText.c_str(), sizeof(buffer));
	//buffer[sizeof(buffer) - 1] = '\0'; // Ensure null-termination

	// Multi-line text input //
	// Change colors for the InputTextMultiline widget
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));  // Yellow text
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.5f, 1.0f)); // Dark blue background
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));  // Red border

	// Define the size of the custom text box
	ImVec2 textBoxSize = ImVec2(-FLT_MIN, -50); // ImVec2(400, 200);

	// Draw a background for the text box
	ImGui::BeginChild("ColoredTextBox", textBoxSize, true, ImGuiWindowFlags_AlwaysUseWindowPadding);

	// Get the draw list for custom rendering
	ImDrawList *drawList = ImGui::GetWindowDrawList();
	ImVec2 startPos = ImGui::GetCursorScreenPos();

	// Line height for spacing
	float lineHeight = ImGui::GetTextLineHeight();

	// Render each line with its respective color
	StormTelnMessageEl *el = 0;
	VString str;
	ImVec4 color;
	int line = 0;
	
	while (steln_outbuf.Next(el, str, color)) {
		// Add some content inside the child window
		ImGui::TextColored(color, str);
		//ImGui::Text("This is normal text inside the child window.");

		//drawList->AddText(ImVec2(startPos.x, startPos.y + line * lineHeight), ImGui::GetColorU32(color), str);
		line++;
	}

	//for (size_t i = 0; i < 2; ++i) {
	//	//const ColoredText &line = coloredLines[i];
	//	drawList->AddText(ImVec2(startPos.x, startPos.y + i * lineHeight), ImGui::GetColorU32(ImVec4(0.0f, 0.0f, 1.0f, 1.0f)), "12345 ");
	//}

	ImGui::EndChild();

	/*
	if (ImGui::InputTextMultiline("##MultilineText", storm_buf, storm_buf.GetMaxSize(), ImVec2(-FLT_MIN, -50))) {
		// Update the global variable when the text changes
		//multiLineText = buffer;
		storm_buf.CheckStrSize();
	}*/

	// Restore default colors
	ImGui::PopStyleColor(3);

	// Send data Button //
	if (ImGui::Button("Send")) {
		// Event: Button was clicked
		//inputText = buffer; // Save the entered text into the global variable
		//ImGui::Text("Button clicked! Text: %s", storm_url); // Display a message with the entered text
		steln_input.CheckStrSize();

		steln_inbuf.AddStr(steln_input.GetStr());
		steln_inbuf.AddStr("\r\n");

		steln_input.Clean();
	}

	ImGui::SameLine();

	// Input text field //	
// ImGui::InputText creates a text input field where the user can type
// The flag ImGuiInputTextFlags_EnterReturnsTrue makes the function return true when Enter is pressed
	if (ImGui::InputText("##Send", steln_input, steln_input.GetMaxSize(), ImGuiInputTextFlags_EnterReturnsTrue)) {
		// Event: Enter key was pressed
		steln_input.CheckStrSize();		

		if (steln_r)
			steln_input.AddStr("\r");

		if (steln_n)
			steln_input.AddStr("\n");

		steln_inbuf.AddStr(steln_input.GetStr());

		steln_input.Clean();
	}

	ImGui::SameLine();
	ImGui::Checkbox("\\r", &steln_r);
	ImGui::SameLine();
	ImGui::Checkbox("\\n", &steln_n);

	ImGui::End();
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




