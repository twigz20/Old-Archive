#ifndef  PACKET_TAGS_H
#define  PACKET_TAGS_H

//unsigned short ServerPort = 9094;

namespace Server
{
	// Packets originated in the server
	enum PacketType
	{
		BroadcastMessage = 0,	// format: [Int32:packetType] [string:message]
		SpawnSelf,
		InitialState,
		PlayerRealtimeChange,
		PlayerEvent,
		PlayerConnect,
		PlayerDisconnect,
		UpdateClientState,
		PointScored,
		BallUpdate
	};
}

namespace Client
{
	// Packets originated in the client
	enum PacketType
	{
		PlayerEvent = 10,
		PlayerRealtimeChange,
		PositionUpdate,
		NewConnection,
		GameEvent,
		Acknowledgement,
		Quit
	};
}

namespace PlayerActions
{
	enum Action
	{
		MoveUp,
		MoveDown,
		ActionCount
	};
}

namespace PacketTags {
	enum PFlag {
		None,
		Ping,
		Pong,
		Connect,
		Disconnect,
		Connected,
		Disconnected,
		Text,
		Move,
		Ball,
		Paddle,
		Score,
		Name,
		Confirm,
		Player1,
		Player2,
		TwoPlayersConnected
		// ... Add your flags here
	};
};

#endif // ! PACKET_TAGS_H