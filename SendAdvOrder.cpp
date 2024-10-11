#include "pch.h"
#include "SendAdvOrder.h"
#include "ClientMQL.h"

long SendAdvOrder(wchar_t* sym, wchar_t* sec, wchar_t* cur, wchar_t* exch, int direction, wchar_t* vol, double sl, double tp)
{
	Order mainOrder, firstChild, secondChild;

	// Connect to TWS or IB Gateway
	AdvOrder client("127.0.0.1", 7496, 0);

	if (!client.isConnected())
	{
		return 0;
	}

	// Access time
	client.reqCurrentTime();
	std::this_thread::sleep_for(std::chrono::seconds(2));
	client.signal.waitForSignal();
	client.reader->processMsgs();

	// Define the contract
	Contract con = Contract();          //sym

	char s_sym[64],
		s_sec[64],
		s_cur[64],
		s_exch[64],
		s_vol[64];

	sprintf_s(s_sym, wcslen(sym) + 1, "%S", sym);
	sprintf_s(s_sec, wcslen(sec) + 1, "%S", sec);
	sprintf_s(s_cur, wcslen(cur) + 1, "%S", cur);
	sprintf_s(s_exch, wcslen(exch) + 1, "%S", exch);
	sprintf_s(s_vol, wcslen(vol) + 1, "%S", vol);

	con.symbol = s_sym;					// "AF"
	con.secType = s_sec;				// "CFD"
	con.currency = s_cur;				// "EUR"
	con.exchange = s_exch;				// "SMART"

	// Access contract details
	client.reqContractDetails(0, con);
	std::this_thread::sleep_for(std::chrono::seconds(2));
	client.signal.waitForSignal();
	client.reader->processMsgs();


	// Create the bracket order
	mainOrder.orderId = client.orderId;
	mainOrder.action = direction == BUY ? "BUY" : "SELL";					// direction
	mainOrder.orderType = "MKT";
	mainOrder.totalQuantity = DecimalFunctions::stringToDecimal(s_vol);		// vol
	mainOrder.transmit = true;

	// Limit order child
	firstChild.orderId = client.orderId + 1;
	firstChild.action = direction == BUY ? "SELL" : "BUY";					// direction
	firstChild.orderType = "LMT";
	firstChild.totalQuantity = DecimalFunctions::stringToDecimal(s_vol);	// vol
	firstChild.lmtPrice = tp;												// tp
	firstChild.parentId = client.orderId;
	firstChild.transmit = true;

	// Stop order child
	secondChild.orderId = client.orderId + 2;
	secondChild.action = direction == BUY ? "SELL" : "BUY";					// direction
	secondChild.orderType = "STP";
	secondChild.totalQuantity = DecimalFunctions::stringToDecimal(s_vol);	// vol
	secondChild.auxPrice = sl;												// sl
	secondChild.parentId = client.orderId;
	secondChild.transmit = true;

	// Get an order ID
	client.reqIds(1000);
	std::this_thread::sleep_for(std::chrono::seconds(1));
	client.signal.waitForSignal();
	client.reader->processMsgs();

	// Place the order
	client.placeOrder(client.orderId, con, mainOrder);
	client.placeOrder(client.orderId + 1, con, firstChild);
	client.placeOrder(client.orderId + 2, con, secondChild);
	std::this_thread::sleep_for(std::chrono::seconds(1));
	client.signal.waitForSignal();
	client.reader->processMsgs();

	// Disconnect
	client.eDisconnect();

	return client.tm_m;	// return sucess time
}
