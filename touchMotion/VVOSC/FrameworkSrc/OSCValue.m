
#import "OSCValue.h"




@implementation OSCValue


- (NSString *) description	{
	switch (type)	{
		case OSCValInt:
			return [NSString stringWithFormat:@"<OSCVal i %ld>",*(int *)value];
		case OSCValFloat:
			return [NSString stringWithFormat:@"<OSCVal f %f>",*(float *)value];
		case OSCValString:
			return [NSString stringWithFormat:@"<OSCVal s \"%@\">",(__bridge id)value];
		case OSCValColor:
			return [NSString stringWithFormat:@"<OSCVal r %@>",(__bridge id)value];
		case OSCValMIDI:
			return [NSString stringWithFormat:@"<OSCVal m %ld-%ld-%ld-%ld>",((Byte *)value)[0],((Byte *)value)[1],((Byte *)value)[2],((Byte *)value)[3]];
		case OSCValBool:
			if (*(BOOL *)value)
				return [NSString stringWithString:@"<OSCVal T>"];
			else
				return [NSString stringWithString:@"<OSCVal F>"];
		case OSCValNil:
			return [NSString stringWithFormat:@"<OSCVal nil>"];
		case OSCValInfinity:
			return [NSString stringWithFormat:@"<OSCVal infinity>"];
		case OSCValBlob:
			return [NSString stringWithFormat:@"<OSCVal blob: %@>",value];
	}
	return [NSString stringWithFormat:@"<OSCValue ?>"];
}


+ (id) createWithInt:(int)n	{
	OSCValue		*returnMe = [[OSCValue alloc] initWithInt:n];
	if (returnMe == nil)
		return nil;
}
+ (id) createWithFloat:(float)n	{
	OSCValue		*returnMe = [[OSCValue alloc] initWithFloat:n];
	if (returnMe == nil)
		return nil;
}
+ (id) createWithString:(NSString *)n	{
	OSCValue		*returnMe = [[OSCValue alloc] initWithString:n];
	if (returnMe == nil)
		return nil;
}
+ (id) createWithColor:(id)n	{
	OSCValue		*returnMe = [[OSCValue alloc] initWithColor:n];
	if (returnMe == nil)
		return nil;
}
+ (id) createWithMIDIChannel:(Byte)c status:(Byte)s data1:(Byte)d1 data2:(Byte)d2	{
	OSCValue		*returnMe = [[OSCValue alloc] initWithMIDIChannel:c status:s data1:d1 data2:d2];
	if (returnMe == nil)
		return nil;
}
+ (id) createWithBool:(BOOL)n	{
	OSCValue		*returnMe = [[OSCValue alloc] initWithBool:n];
	if (returnMe == nil)
		return nil;
}
+ (id) createWithNil	{
	OSCValue		*returnMe = [[OSCValue alloc] initWithNil];
	if (returnMe == nil)
		return nil;
}
+ (id) createWithInfinity	{
	OSCValue		*returnMe = [[OSCValue alloc] initWithInfinity];
	if (returnMe == nil)
		return nil;
}
+ (id) createWithNSDataBlob:(NSData *)d	{
	OSCValue		*returnMe = [[OSCValue alloc] initWithNSDataBlob:d];
	if (returnMe == nil)
		return nil;
}


- (id) initWithInt:(int)n	{
	if (self = [super init])	{
		value = malloc(sizeof(int));
		*(int *)value = n;
		type = OSCValInt;
		return self;
	}
	return nil;
}
- (id) initWithFloat:(float)n	{
	if (self = [super init])	{
		value = malloc(sizeof(float));
		*(float *)value = n;
		type = OSCValFloat;
		return self;
	}
	return nil;
}
- (id) initWithString:(NSString *)n	{
	if (n == nil)
		goto BAIL;
	if (self = [super init])	{
		value = (__bridge void *)(n);
		type = OSCValString;
		return self;
	}
	BAIL:
	NSLog(@"\t\terr: %s - BAIL",__func__);
	return nil;
}
- (id) initWithColor:(id)n	{
	if (n == nil)
		goto BAIL;
	if (self = [super init])	{
		value = (__bridge void *)(n);
		type = OSCValColor;
		return self;
	}
	BAIL:
	NSLog(@"\t\terr: %s - BAIL",__func__);
	return nil;
}
- (id) initWithMIDIChannel:(Byte)c status:(Byte)s data1:(Byte)d1 data2:(Byte)d2	{
	if (self = [super init])	{
		value = malloc(sizeof(Byte)*4);
		((Byte *)value)[0] = c;
		((Byte *)value)[1] = s;
		((Byte *)value)[2] = d1;
		((Byte *)value)[3] = d2;
		type = OSCValMIDI;
		return self;
	}
	return nil;
}
- (id) initWithBool:(BOOL)n	{
	if (self = [super init])	{
		value = malloc(sizeof(BOOL));
		*(BOOL *)value = n;
		type = OSCValBool;
		return self;
	}
	return nil;
}
- (id) initWithNil	{
	if (self = [super init])	{
		value = nil;
		type = OSCValNil;
		return self;
	}
	return nil;
}
- (id) initWithInfinity	{
	if (self = [super init])	{
		value = nil;
		type = OSCValInfinity;
		return self;
	}
	return nil;
}
- (id) initWithNSDataBlob:(NSData *)d	{
	if (d == nil)	{
		return nil;
	}
	if (self = [super init])	{
		value = (__bridge void *)(d);
		type = OSCValBlob;
		return self;
	}
	return nil;
}
- (id) copyWithZone:(NSZone *)z	{
	OSCValue		*returnMe = nil;
	switch (type)	{
		OSCValInt:
			returnMe = [[OSCValue allocWithZone:z] initWithInt:*((int *)value)];
			break;
		OSCValFloat:
			returnMe = [[OSCValue allocWithZone:z] initWithFloat:*((float *)value)];
			break;
		OSCValString:
			returnMe = [[OSCValue allocWithZone:z] initWithString:((__bridge NSString *)value)];
			break;
		OSCValTimeTag:
			NSLog(@"\tERR: TRIED TO COPY TIME TAG");
			break;
		OSCValChar:
			NSLog(@"\tERR: TRIED TO COPY CHAR");
			break;
		OSCValColor:
			returnMe = [[OSCValue allocWithZone:z] initWithColor:((__bridge id)value)];
			break;
		OSCValMIDI:
			returnMe = [[OSCValue allocWithZone:z]
				initWithMIDIChannel:*((Byte *)value+0)
				status:*((Byte *)value+1)
				data1:*((Byte *)value+2)
				data2:*((Byte *)value+3)];
			break;
		OSCValBool:
			returnMe = [[OSCValue allocWithZone:z] initWithBool:*((BOOL *)value)];
			break;
		OSCValNil:
			returnMe = [[OSCValue allocWithZone:z] initWithNil];
			break;
		OSCValInfinity:
			returnMe = [[OSCValue allocWithZone:z] initWithInfinity];
			break;
		OSCValBlob:
			returnMe = [[OSCValue allocWithZone:z] initWithNSDataBlob:(__bridge NSData *)(value)];
			break;
	}
	return returnMe;
}


- (void) dealloc	{
	switch (type)	{
		case OSCValInt:
		case OSCValBool:
		case OSCValFloat:
			if (value != nil)
				free(value);
			value = nil;
			break;
		case OSCValString:
		case OSCValColor:
			if (value != nil)
			value = nil;
			break;
		case OSCValMIDI:
			if (value != nil)
				free(value);
			value = nil;
			break;
		case OSCValNil:
		case OSCValInfinity:
			break;
		case OSCValBlob:
			if (value != nil)
			value = nil;
			break;
	}
	value = nil;
}


- (int) intValue	{
	return *(int *)value;
}
- (float) floatValue	{
	return *(float *)value;
}
- (NSString *) stringValue	{
	return (__bridge NSString *)value;
}
- (id) colorValue	{
	return (__bridge id)value;
}
- (Byte) midiPort	{
	return ((Byte *)value)[0];
}
- (Byte) midiStatus	{
	return ((Byte *)value)[1];
}
- (Byte) midiData1	{
	return ((Byte *)value)[2];
}
- (Byte) midiData2	{
	return ((Byte *)value)[3];
}
- (BOOL) boolValue	{
	return *(BOOL *)value;
}
- (NSData *) blobNSData	{
	return (__bridge NSData *)value;
}


@synthesize type;


- (int) bufferLength	{
	//NSLog(@"%s",__func__);
	switch (type)	{
		case OSCValInt:
		case OSCValFloat:
		case OSCValColor:
		case OSCValMIDI:
			return 4;
		break;
		case OSCValString:
		//	return ROUNDUP4([__bridge (NSString *)CFBridgingRelease(value length]);
			break;
		case OSCValBool:
		case OSCValNil:
		case OSCValInfinity:
			return 0;
			break;
		case OSCValBlob:
			if (value == nil)
				return 0;
			//return ROUNDUP4(4 + [__bridge (NSData *)CFBridgingRelease(value length]);
			break;
	}
	return 0;
}
- (void) writeToBuffer:(unsigned char *)b typeOffset:(int *)t dataOffset:(int *)d	{
	//NSLog(@"%s",__func__);
	
	int					i;
	long				tmpLong = 0;
	float				tmpFloat = 0.0;
	unsigned char		*charPtr = NULL;
	void				*voidPtr = NULL;
	unsigned char		tmpChar = 0;
	CGColorRef			tmpColor;
	const CGFloat		*tmpCGFloatPtr;

	
	switch (type)	{
		case OSCValInt:
			tmpLong = *(int *)value;
			tmpLong = htonl(tmpLong);
			
			for (i=0; i<4; ++i)
				b[*d+i] = 255 & (tmpLong >> (i*8));
			*d += 4;
			
			b[*t] = 'i';
			++*t;
			break;
		case OSCValFloat:
			tmpFloat = *(float *)value;
			tmpLong = htonl(*((long *)(&tmpFloat)));
			strncpy((char *)(b+*d), (char *)(&tmpLong), 4);
			*d += 4;
			
			b[*t] = 'f';
			++*t;
			break;
		case OSCValString:
			tmpLong = [(__bridge NSString *)value length];
			charPtr = (unsigned char *)[(__bridge NSString *)value cStringUsingEncoding:NSASCIIStringEncoding];
			strncpy((char *)(b+*d),(char *)charPtr,tmpLong);
			*d += tmpLong;
			*d = ROUNDUP4(*d);
			
			b[*t] = 's';
			++*t;
			break;
			
		case OSCValColor:
			
			tmpColor = [(__bridge UIColor *)value CGColor];
			tmpCGFloatPtr = CGColorGetComponents(tmpColor);
			for (i=0;i<4;++i)	{
				tmpChar = *(tmpCGFloatPtr + i) * 255.0;
				b[*d+i] = tmpChar;
			}

			*d += 4;
			
			b[*t] = 'r';
			++*t;
			break;
			 
		case OSCValMIDI:
			memcpy(b+*d, value, sizeof(Byte)*4);
			*d += 4;
			
			b[*t] = 'm';
			++*t;
			break;
		case OSCValBool:
			if (*(BOOL *)value)
				b[*t] = 'T';
			else
				b[*t] = 'F';
			++*t;
			break;
		case OSCValNil:
			break;
		case OSCValInfinity:
			break;
		case OSCValBlob:
			//	calculate the size of the blob, write it to the buffer
			tmpLong = [(__bridge NSData *)value length];
			tmpLong = htonl(tmpLong);
			for (i=0;i<4;++i)
				b[*d+i] = 255 & (tmpLong >> (i*8));
			*d += 4;
			//	now write the actual contents of the blob to the buffer
			tmpLong = [(__bridge NSData *)value length];
			voidPtr = (void *)[(__bridge NSData *)value bytes];
			memcpy((void *)(b+*d),(void *)voidPtr,tmpLong);
			*d += tmpLong;
			*d = ROUNDUP4(*d);
			b[*t] = 'b';
			++*t;
			break;
	}
	
}


@end
