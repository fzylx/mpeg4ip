/*
 * Copyright (c) 1999 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * Copyright (c) 1999 Apple Computer, Inc.  All Rights Reserved.
 * The contents of this file constitute Original Code as defined in and are 
 * subject to the Apple Public Source License Version 1.1 (the "License").  
 * You may not use this file except in compliance with the License.  Please 
 * obtain a copy of the License at http://www.apple.com/publicsource and 
 * read it before using this file.
 * 
 * This Original Code and all software distributed under the License are 
 * distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER 
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES, 
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS 
 * FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT.  Please see the License for 
 * the specific language governing rights and limitations under the 
 * License.
 * 
 * 
 * @APPLE_LICENSE_HEADER_END@
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "QTRTPFile.h"

//extern char		*optarg;
//extern int		optind;
	
int main(int argc, char *argv[]) {
	// Temporary vars
	int			ch;
	struct timeval	tp;

	// General vars
	bool			Debug = false, DeepDebug = false;

	const char		*IPAddress;
	const char		*BasePort;

	const char		*MovieFilename;

	int				s;
	QTRTPFile		*RTPFile;

	int				CurPort;
	Float64			StartTime;


	//
	// Read our command line options
	while( (ch = getopt(argc, argv, "dD")) != -1 ) {
		switch( ch ) {
			case 'd':
				Debug = true;
			break;

			case 'D':
				Debug = true;
				DeepDebug = true;
			break;
		}
	}

	argc -= optind;
	argv += optind;

	//
	// Validate our arguments.
	if( argc < 4 ) {
		printf("usage: QTBroadcaster <ip address> <baseport> <filename> <track#n> <track#n+1> ..\n");
		exit(1);
	}
	
	IPAddress = *argv++; argc--;
	BasePort = *argv++; argc--;
	MovieFilename = *argv++; argc--;


	//
	// Open the movie.
	RTPFile = new QTRTPFile(Debug, DeepDebug);
	switch( RTPFile->Initialize(MovieFilename) ) {
		case QTRTPFile::errNoError:
		break;

		case QTRTPFile::errFileNotFound:
			printf("Error!  File not found \"%s\"!\n", MovieFilename);
			exit(1);
		case QTRTPFile::errNoHintTracks:
			printf("Error!  No hint tracks \"%s\"!\n", MovieFilename);
			exit(1);
		case QTRTPFile::errInvalidQuickTimeFile:
			printf("Error!  Invalid movie file \"%s\"!\n", MovieFilename);
			exit(1);
		case QTRTPFile::errInternalError:
			printf("Error!  Internal error opening movie file \"%s\"!\n", MovieFilename);
			exit(1);
	}
	
	
	//
	// Add the tracks that we're interested in.
	CurPort = atoi(BasePort);
	while(argc--) {
		switch( RTPFile->AddTrack(atoi(*argv)) ) {
			case QTRTPFile::errNoError:
			break;

			case QTRTPFile::errFileNotFound:
			case QTRTPFile::errNoHintTracks:
			case QTRTPFile::errInvalidQuickTimeFile:
			case QTRTPFile::errInternalError:
				printf("Error!  Invalid movie file \"%s\"!\n", MovieFilename);
				exit(1);
		}

		RTPFile->SetTrackCookie(atoi(*argv), (char *)CurPort);
		CurPort += 2;

		(void)RTPFile->GetSeekTimestamp(atoi(*argv));
		argv++;
	}
	
	
	//
	// Create our socket to broadcast to.
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if( s == -1 ) {
		printf("Error!  Couldn't create socket!\n");
		exit(1);
	}
	
	
	//
	// Seek to the beginning of the movie.
	if( RTPFile->Seek(0.0) != QTRTPFile::errNoError ) {
		printf("Error!  Couldn't seek to time 0.0!\n");
		exit(1);
	}
	

	//
	// Send packets..
	gettimeofday(&tp, NULL);
	StartTime = tp.tv_sec + ((Float64)tp.tv_usec / 1000000);
	while(1) {
		// General vars
		char	*Packet;
		int		PacketLength;
		int		Cookie;

		Float64	SleepTime;

		struct sockaddr_in	sin;
		

		//
		// Get the next packet.
		Float64 TransmitTime = RTPFile->GetNextPacket(&Packet, &PacketLength, (void **)&Cookie);
		if( Packet == NULL )
			break;
		
		//
		// Wait until it is time to send the packet.
		gettimeofday(&tp, NULL);
		SleepTime = tp.tv_sec + ((Float64)tp.tv_usec / 1000000);
		SleepTime = (StartTime + TransmitTime) - SleepTime;
		if( SleepTime > 0.0 ) {
printf("Sleeping for %.2f seconds (TransmitTime=%.2f).\n", SleepTime, TransmitTime);
			usleep((unsigned int)(SleepTime * 1000000));
		}

		//
		// Send the packet.
		memset(&sin, 0, sizeof(struct sockaddr_in));
		sin.sin_family = AF_INET;
		sin.sin_port = htons(Cookie);
		sin.sin_addr.s_addr = inet_addr(IPAddress);
		sendto(s, Packet, PacketLength, 0, (struct sockaddr *)&sin, sizeof(struct sockaddr));
	}

	
	//
	// Close the socket.
	close(s);

	//
	// Close our RTP file.
	delete RTPFile;

	return 0;
}