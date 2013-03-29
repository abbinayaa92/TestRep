
#import "NamedMutLockArray.h"




@implementation NamedMutLockArray


+ (id) arrayWithCapacity:(int)c	{
	NamedMutLockArray		*returnMe = [[NamedMutLockArray alloc] initWithCapacity:c];
	if (returnMe == nil)
		return nil;
	
}
+ (id) create	{
	NamedMutLockArray		*returnMe = [[NamedMutLockArray alloc] initWithCapacity:0];
	if (returnMe == nil)
		return nil;
	
}


- (NSString *) description	{
	return [NSString stringWithFormat:@"<NamedMutLockArray: %@, %@>",name,array];
}
- (id) initWithCapacity:(NSUInteger)c	{
	if (self = [super initWithCapacity:c])	{
		name = nil;
		return self;
	}
	
	return nil;
}
- (id) init	{
	return [self initWithCapacity:0];
}

- (NSComparisonResult) nameCompare:(NamedMutLockArray *)comp	{
	if (name==nil)
		return NSOrderedDescending;
	NSString		*compName = [comp name];
	if (compName==nil)
		return NSOrderedAscending;
	return [name caseInsensitiveCompare:compName];
}


- (void) setName:(NSString *)n	{

}
- (NSString *)name	{
	return name;
}


@end
