/*
    File:       main.m

    Contains:   A command line tool to exercise the UDPEcho46 class.

    Written by: DTS

    Copyright:  Copyright (c) 2010 Apple Inc. All Rights Reserved.

    Disclaimer: IMPORTANT: This Apple software is supplied to you by Apple Inc.
                ("Apple") in consideration of your agreement to the following
                terms, and your use, installation, modification or
                redistribution of this Apple software constitutes acceptance of
                these terms.  If you do not agree with these terms, please do
                not use, install, modify or redistribute this Apple software.

                In consideration of your agreement to abide by the following
                terms, and subject to these terms, Apple grants you a personal,
                non-exclusive license, under Apple's copyrights in this
                original Apple software (the "Apple Software"), to use,
                reproduce, modify and redistribute the Apple Software, with or
                without modifications, in source and/or binary forms; provided
                that if you redistribute the Apple Software in its entirety and
                without modifications, you must retain this notice and the
                following text and disclaimers in all such redistributions of
                the Apple Software. Neither the name, trademarks, service marks
                or logos of Apple Inc. may be used to endorse or promote
                products derived from the Apple Software without specific prior
                written permission from Apple.  Except as expressly stated in
                this notice, no other rights or licenses, express or implied,
                are granted by Apple herein, including but not limited to any
                patent rights that may be infringed by your derivative works or
                by other works in which the Apple Software may be incorporated.

                The Apple Software is provided by Apple on an "AS IS" basis. 
                APPLE MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING
                WITHOUT LIMITATION THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
                MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, REGARDING
                THE APPLE SOFTWARE OR ITS USE AND OPERATION ALONE OR IN
                COMBINATION WITH YOUR PRODUCTS.

                IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT,
                INCIDENTAL OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
                TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
                DATA, OR PROFITS; OR BUSINESS INTERRUPTION) ARISING IN ANY WAY
                OUT OF THE USE, REPRODUCTION, MODIFICATION AND/OR DISTRIBUTION
                OF THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER UNDER THEORY
                OF CONTRACT, TORT (INCLUDING NEGLIGENCE), STRICT LIABILITY OR
                OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE POSSIBILITY OF
                SUCH DAMAGE.

*/

#import "UDPEcho.h"

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

#pragma mark * Main

@interface Main : NSObject <UDPEchoDelegate>
{
    UDPEcho *       _echo;
    NSTimer *       _sendTimer;
    NSUInteger      _sendCount;
}
- (BOOL)runServerOnPort:(NSUInteger)port;
- (BOOL)runClientWithHost:(NSString *)host port:(NSUInteger)port;
@end

@interface Main ()
@property (nonatomic, retain, readwrite) UDPEcho *      echo;
@property (nonatomic, retain, readwrite) NSTimer *      sendTimer;
@property (nonatomic, assign, readwrite) NSUInteger     sendCount;
@end

@implementation Main

- (void)dealloc
{
    [self->_echo stop];
    [self->_echo release];
    [self->_sendTimer invalidate];
    [self->_sendTimer release];
    [super dealloc];
}

@synthesize echo      = _echo;
@synthesize sendTimer = _sendTimer;
@synthesize sendCount = _sendCount;

- (BOOL)runServerOnPort:(NSUInteger)port
    // One of two Objective-C 'mains' for this program.  This creates a UDPEcho 
    // object and runs it in server mode.
{
    assert(self.echo == nil);
    
    self.echo = [[[UDPEcho alloc] init] autorelease];
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

    self.echo = [[[UDPEcho alloc] init] autorelease];
    assert(self.echo != nil);
    
    self.echo.delegate = self;

    [self.echo startConnectedToHostName:host port:port];
    
    while (self.echo != nil) {
        [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate distantFuture]];
    }
    
    // The loop above is supposed to run forever.  If it doesn't, something must 
    // have failed and we want main to return EXIT_FAILURE.
    
    return NO;
}

- (void)sendPacket
    // Called by the client code to send a UDP packet.  This is called immediately 
    // after the client has 'connected', and periodically after that from the send 
    // timer.
{
    NSData *    data;

    assert(self.echo != nil);
    assert( ! self.echo.isServer );
    
    data = [[NSString stringWithFormat:@"%zu bottles of beer on the wall", (99 - self.sendCount)] dataUsingEncoding:NSUTF8StringEncoding];
    assert(data != nil);
    
    [self.echo sendData:data];

    self.sendCount += 1;
    if (self.sendCount > 99) {
        self.sendCount = 0;
    }
}

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

- (void)echo:(UDPEcho *)echo didStartWithAddress:(NSData *)address;
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
        [self sendPacket];
        
        assert(self.sendTimer == nil);
        self.sendTimer = [NSTimer scheduledTimerWithTimeInterval:1.0 target:self selector:@selector(sendPacket) userInfo:nil repeats:YES];
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

int main(int argc, char **argv)
{
    #pragma unused(argc)
    #pragma unused(argv)
    int                 retVal;
    BOOL                success;
    NSAutoreleasePool * pool;
    Main *              mainObj;
    int                 port;
    
    pool = [[NSAutoreleasePool alloc] init];
    assert(pool != nil);
    
    retVal = EXIT_FAILURE;
    success = YES;
    if ( (argc >= 2) && (argc <= 3) ) {
        if (argc == 3) {
            port = atoi(argv[2]);
        } else {
            port = 7;
        }
        if ( (port > 0) && (port < 65536) ) {
            if (strcmp(argv[1], "-l") == 0) {
                retVal = EXIT_SUCCESS;

                // server mode
//
              mainObj = [[[Main alloc] init] autorelease];
               assert(mainObj != nil);
//                
               success = [mainObj runServerOnPort:port];
            } else {
                NSString *  hostName;
                
                hostName = [NSString stringWithUTF8String:"172.24.203.104"];
                if (hostName == nil) {
                    fprintf(stderr, "%s: invalid host host: %s\n", getprogname(), "");
                } else {
                    retVal = EXIT_SUCCESS;

                    // client mode

                    mainObj = [[[Main alloc] init] autorelease];
                    assert(mainObj != nil);
                    
                    success = [mainObj runClientWithHost:hostName port:9876];
                }
            }
        }
    }
    
    if (success) {
        if (retVal == EXIT_FAILURE) {
            fprintf(stderr, "usage: %s -l [port]\n",   getprogname());
            fprintf(stderr, "       %s host [port]\n", getprogname());
        }
    } else {
        retVal = EXIT_FAILURE;
    }
    
    [pool drain];
    
    return retVal;
}
