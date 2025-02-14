#define STELN_STATE_NONE		0
#define STELN_STATE_CONNECTING	1
#define STELN_STATE_CONNECTED	2
#define STELN_STATE_DISCONNECT	3
#define STELN_STATE_LISTENING	10
#define STELN_STATE_LISTEN		11
#define STELN_STATE_DISLISTEN	12

// Global
ImGuiCharIdExt<512> storm_url;
ImGuiCharIdExt<S16K> storm_buf;
ImGuiCharIdExt<512> storm_inbuf, storm_input;

class StormTeln : public MSVCOT {
	int state;

	// Connection
	//MString url;
	//StormTelnConnect connect;

public:
	StormTeln() {
		state = STELN_STATE_NONE;

		ConnectPrint(LString() + "Hello from StormTeln!" + " v." + PROJECTVER[0].ver + ".\r\n");
	}

	// Get
	int GetState() {
		return state;
	}

	MString GetTitle() {
		if (state == STELN_STATE_NONE) {
			return "[]";
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

	int DoDisconnect() {
		if (state == STELN_STATE_CONNECTED) {


		}
	}

	// Class one thread
	DWORD COT(LPVOID lp) {
		while (1) {
			if (state == STELN_STATE_CONNECTING)
				ConnectProc();

			Sleep(10);
		}
		return 0;
	}

	void ConnectPrint(VString line){
		if (storm_buf.GetFreeSize() < line.size()) {
			storm_buf.MoveLeft(line.size() - storm_buf.GetFreeSize());
		}

		storm_buf.AddStr(line);
	}

	int ConnectProc(){
		ConIp cip;
		SOCKET sock;
		SString ss;
		char buf[S1K];

		storm_buf.Clean();
		storm_inbuf.Clean();

		ConnectPrint(LString() + "Connecting to: '" + storm_url.GetStr() + "'.\r\n");

		cip.Ip(storm_url.GetStr());

		sock = cip.Connect();

		if(sock <= 0){
			ConnectPrint(LString() + "Connection refused.\r\n");
			state = STELN_STATE_NONE;
			return 0;
		}

		ConnectPrint(LString() + "Connection established.\r\n");
		state = STELN_STATE_CONNECTED;

		while (1) {
			if (ifrecv(sock)) {
				int rcv = recv(sock, buf, sizeof(buf), 0);

				if (rcv < 0) {
					ConnectPrint(LString() + "Connection refused.\r\n");
					state = STELN_STATE_NONE;
					closesocket(sock);
					return 0;
				}

				if (rcv == 0) {
					ConnectPrint(LString() + "Connection closed.\r\n");
					state = STELN_STATE_NONE;
					closesocket(sock);
					return 0;
				}

				ConnectPrint(VString(buf, rcv));
			}

			if (storm_inbuf) {
				int snd = send(sock, storm_inbuf, storm_inbuf.GetSize(), 0);

				if (snd < 0) {
					ConnectPrint(LString() + "Connection refused.\r\n");
					state = STELN_STATE_NONE;
					closesocket(sock);
					return 0;
				}

				if (snd == 0) {
					ConnectPrint(LString() + "Connection closed.\r\n");
					state = STELN_STATE_NONE;
					closesocket(sock);
					return 0;
				}

				storm_inbuf.MoveLeft(snd);
			}
		}


		//unsigned int ishelp = 0, isver = 0, islist = 0, isnobuff = 0, isnoecho = 0;
		/*
		if (isnobuff) BufEchoClass.buff(0);

		if (!islist) {
			cip.Ip(host, bind);
			cl.Print(DrawDLine() + MRGB("ffffff") + Lang("Connect to") + ": '" + host + "'. ");
			if (bind) { cl.Print(DrawDLine() + MRGB("ffffff") + Lang("Bind") + ": '" + bind + "'."); }

			//print(Lang("Connect to"), ": '", host, "'. "); if(bind) print(Lang("Bind"), ": '", bind, "'."); print("\r\n");
			sock = cip.Connect();

			if (sock <= 0) { cl.Print(DrawDLine() + MRGB("ff0000") + Lang("Connection refused") + "."); return 0; }
			cl.Print(DrawDLine() + MRGB("00ff00") + Lang("Connected") + ".\r\n");
		}
		else {
			cip.IpL(host);
			cl.Print(DrawDLine() + MRGB("ffffff") + Lang("Listen") + ": '" + host + "'.\r\n");// if(bind) print(" Bind: '", bind, "'."); print("\r\n");
			asock = cip.Listen();
			if (asock <= 0) { cl.Print(DrawDLine() + MRGB("ff0000") + Lang("Listen fail") + ".\r\n"); return 0; }
		}

		cl.Print(DrawDLine() + MRGB("555555") + "");


		// data
		char buf[S16K], inbuf[S16K]; int inbufs = 0;
		//GlobalConsole gw; //ConsoleWindow cw; gw.NewWnd(&cw);

		while (1) {
			if (islist) {
				if (ifrecv(asock)) {
					sockaddr_in from; int fromlen = sizeof(from);
					sock = accept(asock, (struct sockaddr *)&from, (socklen_t *)&fromlen);
					if (!sock) { cl.Print(DrawDLine() + MRGB("ff0000") + Lang("Accept fail") + "\r\n"); return 0; }
					it.Format("%s: %ip!:%d.\r\n", Lang("Accept connection"), (unsigned int)ntohl(from.sin_addr.s_addr), htons(from.sin_port));
					//print(it);
					cl.Print(DrawDLine() + MRGB("00ff00") + it.ret);
				}
				if (!sock) { Sleep(100); continue; }
			}

			//if (script) { mscr.Run(script); mscr.Sock(sock); }

			while (1) {
				//if (script) { mscr.Do(); }

				if (ifrecv(sock)) {
					int rd = recv(sock, buf, sizeof(buf), 0);
					if (rd < 0) { cl.Print(DrawDLine() + MRGB("550000") + "\r\n" + Lang("Connection refused") + ".\r\n"); }
					else if (rd == 0) { cl.Print(DrawDLine() + MRGB("555500") + "\r\n" + Lang("Connection closed") + ".\r\n"); }
					else cl.Print(DrawDLine() + MRGB("ffffff") + VString(buf, rd)); //print(VString(buf, rd));

					if (rd <= 0) { closesocket(sock); sock = 0; break; }
				}

				contype ct = cl.Read(); unsigned int c;
				if (ct.gett() == CNTKEY && ct.getkeys() & CN_DOWN) {

					c = ct.getxcode(); if (c == 65552) continue;
					inbuf[inbufs++] = c;

					if (!isnoecho) cl.Print(DrawDLine() + MRGB("555555") + VString((char *)&c, 1)); //cw.Drawn(c, MRGB("555555"));
					if (c == '\r') cl.Print(DrawDLine() + MRGB("555555") + "\n"); //cw.Drawn('\n', MRGB("555555"));
					if (c == 8) { inbufs--; if (inbufs) inbufs--; }

					if (c == '\r' || inbufs + 1 >= sizeof(inbuf) || isnobuff) {
						if (c == '\r' && inbufs < sizeof(inbuf)) { inbuf[inbufs++] = '\n'; }
						int se = send(sock, inbuf, inbufs, 0); inbufs = 0;
					}
				}
				Sleep(10);
			}
			if (!islist) break;
		}*/

		return 1;
	}


} StormTeln;