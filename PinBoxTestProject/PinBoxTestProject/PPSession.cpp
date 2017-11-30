#include "PPSession.h"


PPSession::~PPSession()
{
	if (g_network != nullptr) delete g_network;
}


void PPSession::initSession()
{
	if (g_network != nullptr) return;
	g_network = new PPNetwork();
	printf("Init new session.\n");
	//------------------------------------------------------
	// callback when request data is received
	g_network->SetOnReceivedRequest([=](u8* buffer, u32 size, u32 tag) {
		//------------------------------------------------------
		// verify authentication
		//------------------------------------------------------
		if (!g_authenticated)
		{
			if (tag == PPREQUEST_AUTHEN)
			{
				// check for authentication
				PPMessage *authenMsg = new PPMessage();
				if(authenMsg->ParseHeader(buffer))
				{
					if(authenMsg->GetMessageCode() == MSG_CODE_RESULT_AUTHENTICATION_SUCCESS)
					{
						printf("Authentication sucessfully.\n");
						g_authenticated = true;
					}else
					{
						printf("Authentication failed.\n");
						return;
					}
				}else
				{
					printf("Authentication failed.\n");
					return;
				}
				delete authenMsg;
			}
			else
			{
				printf("Client was not authentication.\n");
				return;
			}
		}
		//------------------------------------------------------
		// process data by tag
		switch (tag)
		{
		case PPREQUEST_HEADER:
		{
			if (!g_tmpMessage) g_tmpMessage = new PPMessage();
			if (g_tmpMessage->ParseHeader(buffer))
			{
				//----------------------------------------------------
				// request body part of this message
				g_network->SetRequestData(g_tmpMessage->GetContentSize(), PPREQUEST_BODY);
			}
			else
			{
				delete g_tmpMessage;
				g_tmpMessage = nullptr;
				printf("Parse message header fail. remove message.\n");
			}
			break;
		}
		case PPREQUEST_BODY:
		{
			//------------------------------------------------------
			// if tmp message is null that mean this is useless data then we avoid it
			if (!g_tmpMessage) return;
			// verify buffer size with message estimate size
			if (size == g_tmpMessage->GetContentSize())
			{
				// process message
				switch (g_sessionType)
				{
				case PPSESSION_MOVIE:
				{
					processMovieSession(buffer, size);
					break;
				}
				case PPSESSION_SCREEN_CAPTURE:
				{
					processScreenCaptureSession(buffer, size);
					break;
				}
				case PPSESSION_INPUT_CAPTURE:
				{
					processInputSession(buffer, size);
					break;
				}
				}

				//----------------------------------------------------------
				// Request for next message
				g_network->SetRequestData(MSG_COMMAND_SIZE, PPREQUEST_HEADER);
			}
			//------------------------------------------------------
			// remove message after use
			delete g_tmpMessage;
			g_tmpMessage = nullptr;
			break;
		}
		default: break;
		}
		//------------------------------------------------------
		// free buffer after use
		free(buffer);
	});
}

void PPSession::InitMovieSession()
{
	initSession();
	//--------------------------------------
	// init specific for movie session
	g_sessionType = PPSESSION_MOVIE;
	printf("Init movie session.\n");
}

void PPSession::InitScreenCaptureSession()
{
	initSession();
	//--------------------------------------
	// init specific for movie session
	g_sessionType = PPSESSION_SCREEN_CAPTURE;
	printf("Init screen capture session.\n");
}

void PPSession::InitInputCaptureSession()
{
	initSession();
	//--------------------------------------
	// init specific for movie session
	g_sessionType = PPSESSION_INPUT_CAPTURE;
	printf("Init input session.\n");
}

void PPSession::StartSession(const char* ip, const char* port)
{
	if (g_network == nullptr) return;
	g_network->Start(ip, port);
	g_network->SetOnConnectionSuccessed([=](u8* data, u32 size)
	{
		printf("Note: this called is not on main thread.\n");
		//NOTE: this not called on main thread !
		//--------------------------------------------------
		int8_t code = 0;
		if (g_sessionType == PPSESSION_MOVIE) code = MSG_CODE_REQUEST_AUTHENTICATION_MOVIE;
		else if (g_sessionType == PPSESSION_SCREEN_CAPTURE) code = MSG_CODE_REQUEST_AUTHENTICATION_SCREEN_CAPTURE;
		else if (g_sessionType == PPSESSION_INPUT_CAPTURE) code = MSG_CODE_REQUEST_AUTHENTICATION_INPUT;
		if(code == 0)
		{
			printf("Invalid session type.\n");
			return;
		}
		//--------------------------------------------------
		// screen capture session authen
		PPMessage *authenMsg = new PPMessage();
		authenMsg->BuildMessageHeader(code);
		u8* msgBuffer = authenMsg->BuildMessageEmpty();
		//--------------------------------------------------
		// send authentication message
		g_network->SendMessageData(msgBuffer, authenMsg->GetMessageSize());
		//--------------------------------------------------
		// set request to get result message
		g_network->SetRequestData(MSG_COMMAND_SIZE, PPREQUEST_AUTHEN);
		delete authenMsg;
	});
	g_network->SetOnConnectionClosed([=](u8* data, u32 size)
	{
		//NOTE: this not called on main thread !
		printf("Connection was interupted.\n");
	});
}

void PPSession::CloseSession()
{
	if (g_network == nullptr) return;
	g_network->Stop();
	g_authenticated = false;
	SS_Reset();
}


void PPSession::processMovieSession(u8* buffer, size_t size)
{
	//NOTE: this not called on main thread !
	//------------------------------------------------------
	// process message data	by message type
	//------------------------------------------------------
	switch (g_tmpMessage->GetMessageCode())
	{
	default: break;
	}
}

void PPSession::processScreenCaptureSession(u8* buffer, size_t size)
{
	//NOTE: this not called on main thread !
	//------------------------------------------------------
	// process message data	by message type
	//------------------------------------------------------
	switch (g_tmpMessage->GetMessageCode())
	{
	case MSG_CODE_REQUEST_NEW_SCREEN_FRAME:
	{
		try {
			// get new frame data
			std::cout << "RECEIVED NEW FRAME: " << size << " bytes" << std::endl;

			// If using waiting for new frame then we need:
			//TODO: send request that client received frame and want new one
			if(SS_setting_waitToReceivedFrame)
			{
				//--------------------------------------------------
				// send request that client received frame
				PPMessage *msgObj = new PPMessage();
				msgObj->BuildMessageHeader(MSG_CODE_REQUEST_SCREEN_RECEIVED_FRAME);
				u8* msgBuffer = msgObj->BuildMessageEmpty();
				//--------------------------------------------------
				// send authentication message
				g_network->SendMessageData(msgBuffer, msgObj->GetMessageSize());
				delete msgObj;
			}

			cv::Mat frame;
			std::vector<uchar> data(buffer, buffer + size);
			cv::imdecode(data, 1, &frame);

			// display test raw
			cv::resize(frame, frame, cv::Size(400, 240));
			cv::imshow("Recevied", frame);
			cv::waitKey(1);


			// right way to clean
			std::vector<uchar>().swap(data);
			data.clear();
			data.shrink_to_fit();
			frame.release();

		}catch(cv::Exception& e)
		{
			std::cout << e.msg << std::endl;
		}

		break;
	}
	default: break;
	}
}

void PPSession::processInputSession(u8* buffer, size_t size)
{
	//NOTE: this not called on main thread !
	//------------------------------------------------------
	// process message data	by message type
	//------------------------------------------------------
	switch (g_tmpMessage->GetMessageCode())
	{
	default: break;
	}
}

//-----------------------------------------------------
// screen capture
//-----------------------------------------------------
void PPSession::SS_StartStream()
{
	PPMessage *authenMsg = new PPMessage();
	authenMsg->BuildMessageHeader(MSG_CODE_REQUEST_START_SCREEN_CAPTURE);
	u8* msgBuffer = authenMsg->BuildMessageEmpty();
	g_network->SendMessageData(msgBuffer, authenMsg->GetMessageSize());
	g_network->SetRequestData(MSG_COMMAND_SIZE, PPREQUEST_HEADER);
	SS_v_isStartStreaming = true;
	delete authenMsg;
}

void PPSession::SS_StopStream()
{
	PPMessage *authenMsg = new PPMessage();
	authenMsg->BuildMessageHeader(MSG_CODE_REQUEST_STOP_SCREEN_CAPTURE);
	u8* msgBuffer = authenMsg->BuildMessageEmpty();
	g_network->SendMessageData(msgBuffer, authenMsg->GetMessageSize());
	g_network->SetRequestData(MSG_COMMAND_SIZE, PPREQUEST_HEADER);
	SS_v_isStartStreaming = false;
	delete authenMsg;
}

void PPSession::SS_ChangeSetting()
{
	PPMessage *authenMsg = new PPMessage();
	authenMsg->BuildMessageHeader(MSG_CODE_REQUEST_CHANGE_SETTING_SCREEN_CAPTURE);

	//-----------------------------------------------
	// alloc msg content block
	size_t contentSize = 1;
	u8* contentBuffer = (u8*)malloc(sizeof(u8) * contentSize);
	u8* pointer = contentBuffer;
	//----------------------------------------------
	// setting: wait for received frame
	u8 _setting_waitToReceivedFrame = SS_setting_waitToReceivedFrame ? 1 : 0;
	WRITE_U8(pointer, _setting_waitToReceivedFrame);

	// other setting go here!!!

	//-----------------------------------------------
	// build message
	u8* msgBuffer = authenMsg->BuildMessage(contentBuffer, contentSize);
	g_network->SendMessageData(msgBuffer, authenMsg->GetMessageSize());
	g_network->SetRequestData(MSG_COMMAND_SIZE, PPREQUEST_HEADER);
	delete authenMsg;
}

void PPSession::SS_Reset()
{
	SS_v_isStartStreaming = false;
}
