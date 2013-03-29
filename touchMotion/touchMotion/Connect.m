//
//  Connect.m
//  touchMotion
//
//  Created by Abbinayaa on 8/31/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "Connect.h"
#include <netdb.h>

#pragma mark * Utilities

static NSString * DisplayAddressForAddress(NSData * address)
// Returns a dotted decimal string for the specified address (a (struct sockaddr) 
// within the address NSData).
{
    int         err;
    NSString *  result;
    char        hostStr[NI_MAXHOST];
    char        servStr[NI_MAXSERV];
    
    result = nil;
    
    if (address != nil) {
        
        // If it's a IPv4 address embedded in an IPv6 address, just bring it as an IPv4 
        // address.  Remember, this is about display, not functionality, and users don't 
        // want to see mapped addresses.
        
        if ([address length] >= sizeof(struct sockaddr_in6)) {
            const struct sockaddr_in6 * addr6Ptr;
            
            addr6Ptr = [address bytes];
            if (addr6Ptr->sin6_family == AF_INET6) {
                if ( IN6_IS_ADDR_V4MAPPED(&addr6Ptr->sin6_addr) || IN6_IS_ADDR_V4COMPAT(&addr6Ptr->sin6_addr) ) {
                    struct sockaddr_in  addr4;
                    
                    memset(&addr4, 0, sizeof(addr4));
                    addr4.sin_len         = sizeof(addr4);
                    addr4.sin_family      = AF_INET;
                    addr4.sin_port        = addr6Ptr->sin6_port;
                    addr4.sin_addr.s_addr = addr6Ptr->sin6_addr.__u6_addr.__u6_addr32[3];
                    address = [NSData dataWithBytes:&addr4 length:sizeof(addr4)];
                    assert(address != nil);
                }
            }
        }
        err = getnameinfo([address bytes], (socklen_t) [address length], hostStr, sizeof(hostStr), servStr, sizeof(servStr), NI_NUMERICHOST | NI_NUMERICSERV);
        if (err == 0) {
            result = [NSString stringWithFormat:@"%s:%s", hostStr, servStr];
            assert(result != nil);
        }
    }
    
    return result;
}

static NSString * DisplayStringFromData(NSData *data)
// Returns a human readable string for the given data.
{
    NSMutableString *   result;
    NSUInteger          dataLength;
    NSUInteger          dataIndex;
    const uint8_t *     dataBytes;
    
    assert(data != nil);
    
    dataLength = [data length];
    dataBytes  = [data bytes];
    
    result = [NSMutableString stringWithCapacity:dataLength];
    assert(result != nil);
    
    [result appendString:@"\""];
    for (dataIndex = 0; dataIndex < dataLength; dataIndex++) {
        uint8_t     ch;
        
        ch = dataBytes[dataIndex];
        if (ch == 10) {
            [result appendString:@"\n"];
        } else if (ch == 13) {
            [result appendString:@"\r"];
        } else if (ch == '"') {
            [result appendString:@"\\\""];
        } else if (ch == '\\') {
            [result appendString:@"\\\\"];
        } else if ( (ch >= ' ') && (ch < 127) ) {
            [result appendFormat:@"%c", (int) ch];
        } else {
            [result appendFormat:@"\\x%02x", (unsigned int) ch];
        }
    }
    [result appendString:@"\""];
    
    return result;
}

static NSString * DisplayErrorFromError(NSError *error)
// Given an NSError, returns a short error string that we can print, handling 
// some special cases along the way.
{
    NSString *      result;
    NSNumber *      failureNum;
    int             failure;
    const char *    failureStr;
    
    assert(error != nil);
    
    result = nil;
    
    // Handle DNS errors as a special case.
    
    if ( [[error domain] isEqual:(NSString *)kCFErrorDomainCFNetwork] && ([error code] == kCFHostErrorUnknown) ) {
        failureNum = [[error userInfo] objectForKey:(id)kCFGetAddrInfoFailureKey];
        if ( [failureNum isKindOfClass:[NSNumber class]] ) {
            failure = [failureNum intValue];
            if (failure != 0) {
                failureStr = gai_strerror(failure);
                if (failureStr != NULL) {
                    result = [NSString stringWithUTF8String:failureStr];
                    assert(result != nil);
                }
            }
        }
    }
    
    // Otherwise try various properties of the error object.
    
    if (result == nil) {
        result = [error localizedFailureReason];
    }
    if (result == nil) {
        result = [error localizedDescription];
    }
    if (result == nil) {
        result = [error description];
    }
    assert(result != nil);
    return result;
}


@implementation Connect

Connect* _connect;

@synthesize echo      = _echo;
@synthesize sendTimer = _sendTimer;
@synthesize sendCount = _sendCount;

- (void)dealloc
{
    [self->_echo stop];
    [self->_sendTimer invalidate];
}

- (BOOL)runServerOnPort:(NSUInteger)port
// One of two Objective-C 'mains' for this program.  This creates a UDPEcho 
// object and runs it in server mode.
{
    assert(self.echo == nil);
    
    self.echo = [[UDPEcho alloc] init];
    assert(self.echo != nil);
    
    self.echo.delegate = self;
    
    [self.echo startServerOnPort:port];
    
    while (self.echo != nil) {
        [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate distantFuture]];
    }
    
    // The loop above is supposed to run forever.  If it doesn't, something must 
    // have failed and we want main to return EXIT_FAILURE.
    
    return NO;
}

- (BOOL)runClientWithHost:(NSString *)host port:(NSUInteger)port
// One of two Objective-C 'mains' for this program.  This creates a UDPEcho 
// object in client mode, talking to the specified host and port, and then 
// periodically sends packets via that object.
{
    assert(host != nil);
    assert( (port > 0) && (port < 65536) );
    
    assert(self.echo == nil);
    
    self.echo = [[UDPEcho alloc] init];
    assert(self.echo != nil);
    
    self.echo.delegate = self;
    
    [self.echo startConnectedToHostName:host port:port];
   
    _connect=self;
//    while (self.echo != nil) {
//        [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate distantFuture]];
//    }
    
    // The loop above is supposed to run forever.  If it doesn't, something must 
    // have failed and we want main to return EXIT_FAILURE.
    
    return NO;
}

void sendPacket(NSData* data)
//void sendPacket(float data)
// Called by the client code to send a UDP packet.  This is called immediately 
// after the client has 'connected', and periodically after that from the send 
// timer.
{
    //NSData *    data;
    
    assert(_connect.echo != nil);
    assert( ! _connect.echo.isServer );
    
   // data = [[NSString stringWithFormat:@"%zu bottles of beer on the wall", (99 - self.sendCount)] dataUsingEncoding:NSUTF8StringEncoding];
   // assert(data != nil);
  //  NSData *myData = [NSData dataWithBytes:&data length:sizeof(data)];
    [_connect.echo sendData:data];
    
//    self.sendCount += 1;
//    if (self.sendCount > 99) {
//        self.sendCount = 0;
//    }
   
}

//void send_Packet(float x, float y) {
//   
//        MonsteriOSCMessage *msg = [[MonsteriOSCMessage alloc] initWithAddress:@"/foot"];
//        [msg addFloat:x];
//        [msg addFloat:y]; // TODO: add double type
//        
//        MonsteriOSCPacket *packet = [[MonsteriOSCPacket alloc] initWithContent:msg];
//        
//        NSLog(@"%f",x);
//        NSLog(@"%f",y);
//        
//        [_connect sendThisPacket:packet];
//    
//}
//
//-(void)sendThisPacket:(MonsteriOSCPacket*)packet {
//   // [MonsteriOSCOutPort sendThisPacket:packet];
//}

- (void)echo:(UDPEcho *)echo didReceiveData:(NSData *)data fromAddress:(NSData *)addr
// This UDPEcho delegate method is called after successfully receiving data.
{
    assert(echo == self.echo);
#pragma unused(echo)
    assert(data != nil);
    assert(addr != nil);
    NSLog(@"received %@ from %@", DisplayStringFromData(data), DisplayAddressForAddress(addr));
}

- (void)echo:(UDPEcho *)echo didReceiveError:(NSError *)error
// This UDPEcho delegate method is called after a failure to receive data.
{
    assert(echo == self.echo);
#pragma unused(echo)
    assert(error != nil);
    NSLog(@"received error: %@", DisplayErrorFromError(error));
}

- (void)echo:(UDPEcho *)echo didSendData:(NSData *)data toAddress:(NSData *)addr
// This UDPEcho delegate method is called after successfully sending data.
{
    assert(echo == self.echo);
#pragma unused(echo)
    assert(data != nil);
    assert(addr != nil);
    NSLog(@"    sent %@ to   %@", DisplayStringFromData(data), DisplayAddressForAddress(addr));
}

- (void)echo:(UDPEcho *)echo didFailToSendData:(NSData *)data toAddress:(NSData *)addr error:(NSError *)error
// This UDPEcho delegate method is called after a failure to send data.
{
    assert(echo == self.echo);
#pragma unused(echo)
    assert(data != nil);
    assert(addr != nil);
    assert(error != nil);
    NSLog(@" sending %@ to   %@, error: %@", DisplayStringFromData(data), DisplayAddressForAddress(addr), DisplayErrorFromError(error));
}

- (void)echo:(UDPEcho *)echo didStartWithAddress:(NSData *)address
// This UDPEcho delegate method is called after the object has successfully started up.
{
    assert(echo == self.echo);
#pragma unused(echo)
    assert(address != nil);
    
    if (self.echo.isServer) {
        NSLog(@"receiving on %@", DisplayAddressForAddress(address));
    } else {
        NSLog(@"sending to %@", DisplayAddressForAddress(address));
    }
    
    if ( ! self.echo.isServer ) {
        //NSData* data = [[NSString stringWithFormat:@"%zu bottles of beer on the wall", (99 - self.sendCount)] dataUsingEncoding:NSUTF8StringEncoding];
       // sendPacket(data);
        
        assert(self.sendTimer == nil);
       // self.sendTimer = [NSTimer scheduledTimerWithTimeInterval:1.0 target:self selector:@selector(sendPacket) userInfo:nil repeats:YES];
    }
}

- (void)echo:(UDPEcho *)echo didStopWithError:(NSError *)error
// This UDPEcho delegate method is called  after the object stops spontaneously.
{
    assert(echo == self.echo);
#pragma unused(echo)
    assert(error != nil);
    NSLog(@"failed with error: %@", DisplayErrorFromError(error));
    self.echo = nil;
}

@end
