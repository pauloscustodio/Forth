//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include <string>
using namespace std;

enum class Error {
	//@@BEGIN: Error
	Ok = 0, // OK
	Abort = -1, // ABORT
	AbortQuote = -2, // ABORTQuote
	StackOverflow = -3, // stack overflow
	StackUnderflow = -4, // stack underflow
	ReturnStackOverflow = -5, // return stack overflow
	ReturnStackUnderflow = -6, // return stack underflow
	DoLoopsNestedTooDeeplyDuringExecution = -7, // do-loops nested too deeply during execution
	DictionaryOverflow = -8, // dictionary overflow
	InvalidMemoryAddress = -9, // invalid memory address
	DivisionByZero = -10, // division by zero
	ResultOutOfRange = -11, // result out of range
	ArgumentTypeMismatch = -12, // argument type mismatch
	UndefinedWord = -13, // undefined word
	InterpretingACompileTimeOnlyWord = -14, // interpreting a compile-only word
	InvalidForget = -15, // invalid FORGET
	AttemptToUseZeroLengthStringAsName = -16, // attempt to use zero-length string as a name
	PicturedNumericOutputStringOverflow = -17, // pictured numeric output string overflow
	ParsedStringOverflow = -18, // parsed string overflow
	DefinitionNameTooLong = -19, // definition name too long
	WriteToReadOnlyLocation = -20, // write to a read-only location
	UnsupportedOperation = -21, // unsupported operation
	ControlStructureMismatch = -22, // control structure mismatch
	AddressAlignmentException = -23, // address alignment exception
	InvalidNumericArgument = -24, // invalid numeric argument
	ReturnStackImbalance = -25, // return stack imbalance
	LoopParametersUnavailable = -26, // loop parameters unavailable
	InvalidRecursion = -27, // invalid recursion
	UserInterrupt = -28, // user interrupt
	CompilerNesting = -29, // compiler nesting
	ObsolescentFeature = -30, // obsolescent feature
	ToBodyUsedOnNonCreatedDefinition = -31, // >BODY used on non-CREATEd definition
	InvalidNameArgument = -32, // invalid name argument (e.g., TO xxx)
	BlockReadException = -33, // block read exception
	BlockWriteException = -34, // block write exception
	InvalidBlockNumber = -35, // invalid block number
	InvalidFilePosition = -36, // invalid file position
	FileIOException = -37, // file I/O exception
	NonExistentFile = -38, // non-existent file
	UnexpectedEndOfFile = -39, // unexpected end of file
	InvalidBaseForFloatingPointConversion = -40, // invalid BASE for floating point conversion
	LossOfPrecision = -41, // loss of precision
	FloatingPointDivideByZero = -42, // floating-point divide by zero
	FloatingPointResultOutOfRange = -43, // floating-point result out of range
	FloatingPointStackOverflow = -44, // floating-point stack overflow
	FloatingPointStackUnderflow = -45, // floating-point stack underflow
	FloatingPointInvalidArgument = -46, // floating-point invalid argument
	CompilationWordListDeleted = -47, // compilation word list deleted
	InvalidPostpone = -48, // invalid POSTPONE
	SearchOrderOverflow = -49, // search-order overflow
	SearchOrderUnderflow = -50, // search-order underflow
	CompilationWordListChanged = -51, // compilation word list changed
	ControlFlowStackOverflow = -52, // control-flow stack overflow
	ExceptionStackOverflow = -53, // exception stack overflow
	FloatingPointUnderflow = -54, // floating-point underflow
	FloatingPointUnidentfiedFault = -55, // floating-point unidentified fault
	Quit = -56, // QUIT
	ExceptionInSendingOrReceivingACharacter = -57, // exception in sending or receiving a character
	IfElseOrThenException = -58, // [IF], [ELSE], or [THEN] exception
	InputBufferOverflow = -256, // input buffer overflow
	InvalidNumber = -257, // invalid number
	OutOfMemory = -258, // out of memory
	StringTooLong = -259, // string too long
	ControlFlowStackUnderflow = -260, // control-flow stack underflow
	TooManyOpenFiles = -261, // too many open files
	InvalidForthCode = -262, // invalid Forth code
	MemoryOverflow = -263, // memory overflow
	NumberOutputOverflow = -264, // number output overflow
	InvalidWordId = -265, // invalid word id
	//@@END
};

void error(Error error_code, const string& arg = "");
