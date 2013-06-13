//
//  NSPLogger.h
//  NSPLib
//
//  Created by Nolan O'Brien on 6/9/13.
//  Copyright (c) 2013 NSProgrammer.com. All rights reserved.
//

#import <Foundation/Foundation.h>

/**
    @def DLOG(format, ...)
    A convenience macro for logging directly to \fn NSLog in \c DEBUG builds. No-op in \c non-DEBUG builds. Log by providing the \c NSPLogLevel. Provide a format string followed by any format arguments.
 */
#if !defined(DEBUG)
#define DLOG(format, ...)     ((void)0)
#else
#define DLOG(format, ...)     NSLog(format,##__VA_ARGS__)
#endif

/**
    @def LOG(lvl, format, ...)
    Log by providing the \c NSPLogLevel. Provide a format string followed by any format arguments.
 */
#define LOG(lvl, format, ...) [NSPLOG writeASync:[NSString stringWithFormat:format, ##__VA_ARGS__] level:lvl]

/**
    @def LOG_HI(format, ...)
    Log using \c NSPLogLevel_HighLevel. Provide a format string followed by any format arguments.
 */
#define LOG_HI(format, ...)  LOG(NSPLogLevel_HighLevel, format,##__VA_ARGS__)
/**
    @def LOG_MID(format, ...)
    Log using \c NSPLogLevel_MidLevel. Provide a format string followed by any format arguments.
 */
#define LOG_MID(format, ...) LOG(NSPLogLevel_MidLevel, format,##__VA_ARGS__)
/**
    @def LOG_LO(format, ...)
    Log using \c NSPLogLevel_LowLevel. Provide a format string followed by any format arguments.
 */
#define LOG_LO(format, ...)  LOG(NSPLogLevel_LowLevel, format,##__VA_ARGS__)

/**
    @def LOG_DBG(format, ...)
    Log at the lowest level in \c DEBUG builds.  No-op on \c non-DEBUG builds. Provide a format string followed by any format arguments.
 */
#ifdef DEBUG
#define LOG_DBG(format, ...) ((void)0)
#else
#define LOG_DBG(format, ...) LOG(NSPLogLevel_LowLevel, format,##__VA_ARGS__)
#endif

/**
    @def NSPLOG
    Helper macro for easy access to the \c NSPLogger class' \fn sharedLog
 */
#define NSPLOG [NSPLogger sharedLog]

/**
    @name NSPLogger Defaults
 */
FOUNDATION_EXPORT NSUInteger const kNSPLoggerDefaultRolloverSize;     /*!< \c 500 writes, not bytes */
FOUNDATION_EXPORT NSUInteger const kNSPLoggerDefaultMaxFiles;         /*!< \c 10 files */
FOUNDATION_EXPORT NSUInteger const kNSPLoggerDefaultWritesPerFlush;   /*!< \c 10 writes per flush */
FOUNDATION_EXPORT NSString*  const kNSPLoggerDefaultFilePrefix;       /*!< \c \@"log." */

/**
    Enum of Log Levels that can be set in \c NSPLogger
 */
typedef enum
{
    NSPLogLevel_Off = 0,    /**< level to indicate logging is off */
    NSPLogLevel_HighLevel,  /**< level for important logs (errors, warnings) */
    NSPLogLevel_MidLevel,   /**< level for un-important logs (status, state changes, information) */
    NSPLogLevel_LowLevel    /**< level for verbose logs that released builds will likely not log (debug logs) */
} NSPLogLevel;

/**
    @class NSPLogger
 
    @par \c NSPLogger is used for logging to disk in a thread safe way.
    @par The \c NSPLogger also logs using a rolling log mechanism so that logging does not overtake the user's disk space.
 */
@interface NSPLogger : NSObject

/** 
    Accessor to the global shared log.
    @par The shared log should not only be used throughout an application but is what the \c NSPLib and \c NSPUILib use for logging.
    @return the globally shared log
    @see setSharedLog:
 */
+ (NSPLogger*) sharedLog;
/**
    Set the globally shared log.
    @param log the \c NSPLogger desired to be used globally as the shared log.
    @see sharedLog
 */
+ (void) setSharedLog:(NSPLogger*)log;

/**
    Same as initWithDirectory:filePrefix:logLevel:writesBeforeRollover:maxFileCount: but with \a prefix, \a writesBeforeRollover and \a fileCount set to default values.
    @see NSPLogger Defaults
    @see initWithDirectory:filePrefix:logLevel:writesBeforeRollover:maxFileCount:
 */
- (id) initWithDirectory:(NSString*)logsDirectory
                logLevel:(NSPLogLevel)level;
/**
    Initialize the \c NSPLogger
    @param logsDirectory the directory on disk to put the logs as they are generated.  It is recommended that this directory be dedicated for the logs and that no other files be in it.
    @param prefix the file name prefix for all logs.  Pass \c nil for \c kNSPLoggerDefaultFilePrefix.  \a prefix can be an empty string.
    @warning throws \c NSDestinationInvalidException when the \a logsDirectory is invalid
    @warning throws \c NSObjectInaccessibleException when a log file cannot be opened
    @see NSPLogger Defaults
 */
- (id) initWithDirectory:(NSString*)logsDirectory
              filePrefix:(NSString*)prefix
                logLevel:(NSPLogLevel)level
    writesBeforeRollover:(NSUInteger)writesBeforeRollover  // UINT32_MAX for unlimited
            maxFileCount:(NSUInteger)fileCount;

/**
    The current operating log level.  Changing the \c logLevel will change what gets logged to the logs.  Duh!
 */
@property (nonatomic, assign) NSPLogLevel logLevel;
/**
    The number of write calls made to the \c NSPLogger before it is flushed to disk.  The less the logs are flushed, the more efficient the logger, however if the logs are not flushed prior to the app terminating (via crash or normally) they will not be availble on disk for future access.
    @see flush
 */
@property (nonatomic, assign) NSUInteger  writesPerFlush;
/**
    The number of write calls made to the \c NSPLogger before the log file rolls over.
    @param writesBeforeRollover provide \c UINT32_MAX for unlimited log files.
    @see logFiles
 */
@property (nonatomic, assign) NSUInteger  writesBeforeRollover;
/**
    The number of log files that can be written to disk before the eldest log file is purged.
    @see logFiles
 */
@property (nonatomic, assign) NSUInteger  maxFileCount;

/**
    flush the logs to disk.  It is recommended to flush the logs on app exit, either via normal termination or crash.
 */
- (void) flush;

/**
    one of the two write methods for writing a log message.  Writes the log asynchronously.  This method is the default method and adds the provided log message to the \c NSPLogger object's writing queue.
    @note This is the default write method to use.
    @param message the string to write to disk.
    @param level the log level for the message.  The message will be filtered based on the \c NSPLogger object's \a logLevel.
    @see writeSync:level:
 */
- (void) writeASync:(NSString*)message level:(NSPLogLevel)level;
/**
    one of the two write methods for writing a log message.  Writes the log synchronously.  This method adds the provided log message to the \c NSPLogger object's writing queue and wait until is finished being written.
    @note It is recommended that any logging made at shutdown time use this method followed by \fn flush.
    @param message the string to write to disk.
    @param level the log level for the message.  The message will be filtered based on the \c NSPLogger object's \a logLevel.
    @see writeASync:level:
    @see flush
 */
- (void) writeSync:(NSString*)message level:(NSPLogLevel)level;  // best for writing out logs at shutdown time

/**
    @return an array of all the current log file paths
 */
- (NSArray*) logFiles;
/**
    @return the path to the directory that contains the logs.
 */
- (NSString*) logDirectoryPath;
/**
    @param maxSizeInBytes minimum of \c 1024 and a maximum of \c UINT32_MAX
    @return the data contained in the tail of all the logs capped with \a maxSizeInBytes
 */
- (NSData*) mostRecentLogs:(NSUInteger)maxSizeInBytes;
/**
    @return the total size of all the log files that this \c NSPLogger encompasses in bytes
*/
- (unsigned long long) totalLogSize;

@end
