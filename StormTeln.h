#define STELN_STATE_NONE		0
#define STELN_STATE_CONNECTING	1
#define STELN_STATE_CONNECTED	2
#define STELN_STATE_DISCONNECT	3
#define STELN_STATE_LISTENING	10
#define STELN_STATE_LISTEN		11
#define STELN_STATE_DISLISTEN	12

class StormTelnMessageEl {
public:
	ImVec4 color;
	unsigned int size;
	unsigned int rn;
	char text[0];
};

class StormTelnMessage {
	ImGuiCharIdExt<S16K> data;

public:
	bool Next(StormTelnMessageEl *&el, VString &str, ImVec4 &color, bool &rn) {
		if (!el) {
			if (!data.GetSize())
				return 0;

			el = (StormTelnMessageEl*) data.GetData();
			str = VString(el->text, el->size);
			color = el->color;
			rn = el->rn;
			return 1;
		}

		char *pel = (char*) el;
		char *eof = data.GetData() + data.GetMaxSize();
		char *eod = data.GetData() + data.GetSize();

		pel += sizeof(StormTelnMessageEl) + el->size;

		if (pel + sizeof(StormTelnMessageEl) > eof)
			return 0;

		if (pel >= eod)
			return 0;

		el = (StormTelnMessageEl*) pel;
		if (pel + sizeof(StormTelnMessageEl) + el->size > eof)
			return 0;

		str = VString(el->text, el->size);
		color = el->color;
		rn = el->rn;

		return 1;
	}

	bool Add(VString str, ImVec4 color) {
		int dsz = sizeof(StormTelnMessageEl) + str.size();
		int fsz = 0;

		if (dsz > data.GetMaxSize()) {
			data.Clean();
			return 0;
		}

		while (data.GetFreeSize() - fsz < sizeof(StormTelnMessageEl) + str.size()) {
			StormTelnMessageEl *el = (StormTelnMessageEl *)(data.GetData() + fsz);
			fsz += sizeof(StormTelnMessageEl) + el->size;
		}

		if (fsz)
			data.MoveLeft(fsz);

		// Add
		char *poi = data.GetData() + data.GetSize();
		StormTelnMessageEl *el = (StormTelnMessageEl *)poi;

		memcpy(el->text, str, str);
		el->color = color;
		el->size = str;
		el->rn = str.endo() == '\r' || str.endo() == '\n';
		
		data.MoveDataSize(dsz);

		return 1;
	}

	void Clean() {
		data.Clean();
	}

};

// Global
ImGuiCharIdExt<512> steln_url;
//StormTelnMessage steln_outbuf;
ImGuiCharIdExt<512> steln_inbuf, steln_input;

bool steln_r = 0, steln_n = 0, steln_stop = 0, steln_echo = 0;

#define STEWLN_COLOR_RED		TextEditor::PaletteIndex::String //	ImVec4(1.0f, 0.0f, 0.0f, 1.0f)
#define STEWLN_COLOR_GREEN		TextEditor::PaletteIndex::Number //	ImVec4(0.0f, 1.0f, 0.0f, 1.0f)
#define STEWLN_COLOR_BLUE		TextEditor::PaletteIndex::Keyword //	ImVec4(0.0f, 0.0f, 1.0f, 1.0f)
#define STEWLN_COLOR_MAGENTA	TextEditor::PaletteIndex::PreprocIdentifier //	ImVec4(1.0f, 0.06f, .75f, 1.0f)
#define STEWLN_COLOR_YELLOW		TextEditor::PaletteIndex::Preprocessor //	ImVec4(1.0f, 1.0f, 0.0f, 1.0f)
#define STEWLN_COLOR_WHITE		TextEditor::PaletteIndex::Punctuation //	ImVec4(1.0f, 1.0f, 1.0f, 1.0f)
#define STEWLN_COLOR_BLACK		TextEditor::PaletteIndex::Breakpoint //	ImVec4(0.0f, 0.0f, 0.0f, 1.0f)


class StormTeln : public MSVCOT, public TLock {
	int state;
	bool opt_newline;

public:
	StormTeln() {
		state = STELN_STATE_NONE;

		ConnectPrint(LString() + "Hello from StormTeln" + " v." + PROJECTVER[0].ver + " (" + PROJECTVER[0].date + ").", STEWLN_COLOR_BLUE);
		ConnectPrint("Enter 'domain:port' for connect or '[ip:]port' for listen.", STEWLN_COLOR_BLUE, 1);
	}

	// Get
	int GetState() {
		return state;
	}

	MString GetTitle() {
		if (state == STELN_STATE_NONE) {
			return "[]";
		}

		if (state == STELN_STATE_CONNECTING || state == STELN_STATE_CONNECTED) {
			return steln_url.GetStr();
		}

		return "";
	}

	// Set

	// Actions
	void DoConnect() {
		if (!cotuse) {
			COTS();
		}

		state = STELN_STATE_CONNECTING;
	}

	void DoListen() {
		if (!cotuse) {
			COTS();
		}

		state = STELN_STATE_LISTENING;
	}

	void DoSend() {
		steln_input.CheckStrSize();

		if (steln_r)
			steln_input.AddStr("\r");

		if (steln_n)
			steln_input.AddStr("\n");

		steln_inbuf.AddStr(steln_input.GetStr());

		if (steln_echo)
			ConnectPrint(steln_input.GetStr(), STEWLN_COLOR_MAGENTA);

		steln_input.Clean();
	}

	void DoDisconnect() {
		if (state == STELN_STATE_CONNECTED || state == STELN_STATE_LISTEN) {
			steln_stop = 1;
		}

		return ;
	}

	// Class one thread
	DWORD COT(LPVOID lp) {
		while (1) {
			if (state == STELN_STATE_CONNECTING)
				ConnectProc();
			if (state == STELN_STATE_LISTENING)
				ListenProc();

			Sleep(10);
		}
		return 0;
	}

	void ConnectPrint(VString line, TextEditor::PaletteIndex color, bool newline = 0){
		UGLOCK(*this);

		/*/ Colors
		for (int i = 0; i < static_cast<int>(TextEditor::PaletteIndex::Max); ++i) {
			editor.AppendText(line, line, static_cast<TextEditor::PaletteIndex>(i));
		}*/

		if(newline && !opt_newline)
			editor.AppendText("\r\n", 2, color);

		opt_newline = line.endo() == '\n';

		editor.AppendText(line, line, color);		
	}

	void ConnectPrintClean() {
		editor.DeleteAll();
		opt_newline = 1;
	}

	int ConnectProc() {
		ConIp cip;
		SOCKET sock;
		SString ss;

		ConnectPrintClean();
		steln_inbuf.Clean();
		steln_stop = 0;

		MsvWndUpdateTitle();
		ConnectPrint(LString() + "Connecting to: '" + steln_url.GetStr() + "'.", STEWLN_COLOR_BLUE, 1);

		cip.Ip(steln_url.GetStr());

		sock = cip.Connect();

		if (sock <= 0) {
			ConnectPrint("Connection refused.", STEWLN_COLOR_RED, 1);
			state = STELN_STATE_NONE;
			return 0;
		}

		ConnectPrint("Connection established.", STEWLN_COLOR_GREEN, 1);
		state = STELN_STATE_CONNECTED;

		DataProc(sock);

		if (state) {
			ConnectPrint("Connection aborted.", STEWLN_COLOR_BLUE, 1);
			state = STELN_STATE_NONE;
		}

		closesocket(sock);

		MsvWndUpdateTitle();

		return 1;
	}

	int ListenProc() {
		ConIp cip;
		SOCKET sock, asock;
		SString ss;

		ConnectPrintClean();
		steln_inbuf.Clean();
		steln_stop = 0;

		MsvWndUpdateTitle();
		ConnectPrint(LString() + "Listen: '" + steln_url.GetStr() + "'.", STEWLN_COLOR_BLUE, 1);

		cip.IpL(steln_url.GetStr());
		asock = cip.Listen();

		if (asock <= 0) {
			state = STELN_STATE_NONE;
			ConnectPrint("Listen fail.", STEWLN_COLOR_RED, 1);
			return 0;
		}

		state = STELN_STATE_LISTEN;

		while (!steln_stop) {
			if (ifrecv(asock)) {
				sockaddr_in from;
				int fromlen = sizeof(from);

				sock = accept(asock, (struct sockaddr *)&from, (socklen_t *)&fromlen);
				if (!sock) {
					state = STELN_STATE_NONE;
					ConnectPrint("Accept fail.", STEWLN_COLOR_RED, 1);
					closesocket(asock);
					return 0;
				}

				ss.Format("Accepted connection: %ip!:%d.\r\n", (unsigned int)ntohl(from.sin_addr.s_addr), htons(from.sin_port));
				ConnectPrint(ss, STEWLN_COLOR_BLUE, 1);

				DataProc(sock);

				closesocket(sock);

			}
			Sleep(10);
		}

		state = STELN_STATE_NONE;
		ConnectPrint("Listen aborted.", STEWLN_COLOR_BLUE, 1);
		
		MsvWndUpdateTitle();

		return 1;
	}

	bool DataProc(SOCKET sock) {
		char buf[S1K];

		ConnectPrint("\r\n", STEWLN_COLOR_BLACK);

		while (!steln_stop) {
			if (ifrecv(sock)) {
				int rcv = recv(sock, buf, sizeof(buf), 0);

				if (rcv < 0) {
					ConnectPrint("Connection refused.", STEWLN_COLOR_RED, 1);
					state = STELN_STATE_NONE;
					closesocket(sock);
					return 0;
				}

				if (rcv == 0) {
					ConnectPrint("Connection closed.", STEWLN_COLOR_BLUE, 1);
					state = STELN_STATE_NONE;
					closesocket(sock);
					return 0;
				}

				ConnectPrint(VString(buf, rcv), STEWLN_COLOR_WHITE, 0);
			}

			if (steln_inbuf) {
				int snd = send(sock, steln_inbuf, steln_inbuf.GetSize(), 0);

				if (snd < 0) {
					ConnectPrint("Connection refused.", STEWLN_COLOR_RED, 1);
					state = STELN_STATE_NONE;
					closesocket(sock);
					return 0;
				}

				if (snd == 0) {
					ConnectPrint("Connection closed.", STEWLN_COLOR_BLUE, 1);
					state = STELN_STATE_NONE;
					closesocket(sock);
					return 0;
				}

				steln_inbuf.MoveLeft(snd);
			}			
		}

		return 1;
	}

} StormTeln;