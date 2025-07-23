//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "errors.h"
#include "forth.h"
#include "stack.h"
#include <iostream>
#include <string>
using namespace std;

static string lookup_error_msg(Error error_code) {
	switch (error_code) {
	//@@BEGIN: ErrorMessage
	case Error::Ok: return "OK"; // 0
	case Error::Abort: return "ABORT"; // -1
	case Error::AbortQuote: return "ABORTQuote"; // -2
	case Error::StackOverflow: return "stack overflow"; // -3
	case Error::StackUnderflow: return "stack underflow"; // -4
	case Error::ReturnStackOverflow: return "return stack overflow"; // -5
	case Error::ReturnStackUnderflow: return "return stack underflow"; // -6
	case Error::DoLoopsNestedTooDeeplyDuringExecution: return "do-loops nested too deeply during execution"; // -7
	case Error::DictionaryOverflow: return "dictionary overflow"; // -8
	case Error::InvalidMemoryAddress: return "invalid memory address"; // -9
	case Error::DivisionByZero: return "division by zero"; // -10
	case Error::ResultOutOfRange: return "result out of range"; // -11
	case Error::ArgumentTypeMismatch: return "argument type mismatch"; // -12
	case Error::UndefinedWord: return "undefined word"; // -13
	case Error::InterpretingACompileTimeOnlyWord: return "interpreting a compile-only word"; // -14
	case Error::InvalidForget: return "invalid FORGET"; // -15
	case Error::AttemptToUseZeroLengthStringAsName: return "attempt to use zero-length string as a name"; // -16
	case Error::PicturedNumericOutputStringOverflow: return "pictured numeric output string overflow"; // -17
	case Error::ParsedStringOverflow: return "parsed string overflow"; // -18
	case Error::DefinitionNameTooLong: return "definition name too long"; // -19
	case Error::WriteToReadOnlyLocation: return "write to a read-only location"; // -20
	case Error::UnsupportedOperation: return "unsupported operation"; // -21
	case Error::ControlStructureMismatch: return "control structure mismatch"; // -22
	case Error::AddressAlignmentException: return "address alignment exception"; // -23
	case Error::InvalidNumericArgument: return "invalid numeric argument"; // -24
	case Error::ReturnStackImbalance: return "return stack imbalance"; // -25
	case Error::LoopParametersUnavailable: return "loop parameters unavailable"; // -26
	case Error::InvalidRecursion: return "invalid recursion"; // -27
	case Error::UserInterrupt: return "user interrupt"; // -28
	case Error::CompilerNesting: return "compiler nesting"; // -29
	case Error::ObsolescentFeature: return "obsolescent feature"; // -30
	case Error::ToBodyUsedOnNonCreatedDefinition: return ">BODY used on non-CREATEd definition"; // -31
	case Error::InvalidNameArgument: return "invalid name argument (e.g., TO xxx)"; // -32
	case Error::BlockReadException: return "block read exception"; // -33
	case Error::BlockWriteException: return "block write exception"; // -34
	case Error::InvalidBlockNumber: return "invalid block number"; // -35
	case Error::InvalidFilePosition: return "invalid file position"; // -36
	case Error::FileIOException: return "file I/O exception"; // -37
	case Error::NonExistentFile: return "non-existent file"; // -38
	case Error::UnexpectedEndOfFile: return "unexpected end of file"; // -39
	case Error::InvalidBaseForFloatingPointConversion: return "invalid BASE for floating point conversion"; // -40
	case Error::LossOfPrecision: return "loss of precision"; // -41
	case Error::FloatingPointDivideByZero: return "floating-point divide by zero"; // -42
	case Error::FloatingPointResultOutOfRange: return "floating-point result out of range"; // -43
	case Error::FloatingPointStackOverflow: return "floating-point stack overflow"; // -44
	case Error::FloatingPointStackUnderflow: return "floating-point stack underflow"; // -45
	case Error::FloatingPointInvalidArgument: return "floating-point invalid argument"; // -46
	case Error::CompilationWordListDeleted: return "compilation word list deleted"; // -47
	case Error::InvalidPostpone: return "invalid POSTPONE"; // -48
	case Error::SearchOrderOverflow: return "search-order overflow"; // -49
	case Error::SearchOrderUnderflow: return "search-order underflow"; // -50
	case Error::CompilationWordListChanged: return "compilation word list changed"; // -51
	case Error::ControlFlowStackOverflow: return "control-flow stack overflow"; // -52
	case Error::ExceptionStackOverflow: return "exception stack overflow"; // -53
	case Error::FloatingPointUnderflow: return "floating-point underflow"; // -54
	case Error::FloatingPointUnidentfiedFault: return "floating-point unidentified fault"; // -55
	case Error::Quit: return "QUIT"; // -56
	case Error::ExceptionInSendingOrReceivingACharacter: return "exception in sending or receiving a character"; // -57
	case Error::IfElseOrThenException: return "[IF], [ELSE], or [THEN] exception"; // -58
	case Error::InputBufferOverflow: return "input buffer overflow"; // -256
	case Error::InvalidNumber: return "invalid number"; // -257
	case Error::OutOfMemory: return "out of memory"; // -258
	case Error::StringTooLong: return "string too long"; // -259
	case Error::ControlFlowStackUnderflow: return "control-flow stack underflow"; // -260
	case Error::TooManyOpenFiles: return "too many open files"; // -261
	case Error::InvalidForthCode: return "invalid Forth code"; // -262
	case Error::MemoryOverflow: return "memory overflow"; // -263
	case Error::NumberOutputOverflow: return "number output overflow"; // -264
	case Error::InvalidWordId: return "invalid word id"; // -265
	//@@END
	default: return to_string(static_cast<int>(error_code));
	}
}

void error(Error error_code, const string& arg) {
	if (error_code != Error::Ok) {
		string error_msg = lookup_error_msg(error_code);
		cerr << endl << "Error: " << error_msg;
		if (!arg.empty())
			cerr << ": " << arg;
		cerr << endl;
		exit(EXIT_FAILURE);
	}
}

void fTHROW() {
	int error_code = pop();
	error(static_cast<Error>(error_code));
}
