#pragma once

#include <clang-c/Index.h>

#include "InfoStructures/ParsingInfo.h"
#include "Parsing/EntityParser.h"

namespace kodgen
{
	class MethodParser : public EntityParser
	{
		private:
			virtual CXChildVisitResult				setAsCurrentEntityIfValid(CXCursor const& methodAnnotationCursor)	noexcept override final;
			virtual opt::optional<PropertyGroup>	isEntityValid(CXCursor const& currentCursor)						noexcept override final;
			void									setupMethod(CXCursor const& methodCursor, MethodInfo& methodInfo)	noexcept;

		public:
			MethodParser()						= default;
			MethodParser(MethodParser const&)	= default;
			MethodParser(MethodParser&&)		= default;
			~MethodParser()						= default;

			virtual CXChildVisitResult	parse(CXCursor const& cursor)											noexcept override final;

			using EntityParser::startParsing;
	};
}


#include <clang-c/Index.h>

#include "Parsing/EntityParser.h"
#include "Parsing/ParsingResults/MethodParsingResult.h"
#include "Properties/PropertyGroup.h"
#include "Misc/Optional.h"

namespace kodgen
{
	class MethodParser2 : public EntityParser2
	{
		private:
			/**
			*	@brief This method is called at each node (cursor) of the parsing.
			*
			*	@param cursor		AST cursor to the entity to parse.
			*	@param parentCursor	Parent of the current cursor.
			*	@param clientData	Pointer to a data provided by the client. Must contain a MethodParser*.
			*
			*	@return An enum which indicates how to choose the next cursor to parse in the AST.
			*/
			static CXChildVisitResult		parseEntity(CXCursor		cursor,
														CXCursor		parentCursor,
														CXClientData	clientData)					noexcept;

			/**
			*	@brief Retrieve the properties from the provided cursor if possible.
			*
			*	@param cursor Property cursor we retrieve the information from.
			*
			*	@return A filled PropertyGroup if valid, else nullopt.
			*/
			opt::optional<PropertyGroup>	getProperties(CXCursor const& cursor)					noexcept;

			/**
			*	@brief Set the parsed method if it is a valid one.
			*
			*	@param annotationCursor The cursor used to check method validity.
			*
			*	@return An enum which indicates how to choose the next cursor to parse in the AST.
			*/
			CXChildVisitResult				setParsedEntity(CXCursor const& annotationCursor)		noexcept;

			/**
			*	@brief Init the context object of this parser.
			*
			*	@param methodCursor		Root cursor of the method to parse.
			*	@param parsingSettings	ParsingSettings to use to parse methods.
			*	@param propertyParser	PropertyParser to use to parse properties.
			*	@param out_result		Result to fill during parsing.
			*/
			void							initContext(CXCursor const&			methodCursor,
														ParsingSettings const&	parsingSettings,
														PropertyParser&			propertyParser,
														MethodParsingResult&	out_result)			noexcept;

			/**
			*	@brief Fill the provided method information using the currently parsing method.
			*
			*	@param methodInfo MethodInfo structure to fill.
			*/
			void							initializeMethodInfo(MethodInfo& methodInfo)			noexcept;

			/**
			*	@brief Helper to get the ParsingResult contained in the context as a MethodParsingResult.
			*
			*	@return The cast MethodParsingResult.
			*/
			inline MethodParsingResult*		getParsingResult()										noexcept;

		public:
			/**
			*	@brief Parse the method starting at the provided AST cursor.
			*
			*	@param methodCursor		AST cursor to the method to parse.
			*	@param parsingSettings	ParsingSettings used to parse the method.
			*	@param propertyParser	PropertyParser used to parse the method properties.
			*	@param out_result		Result filled while parsing the method.
			*
			*	@return An enum which indicates how to choose the next cursor to parse in the AST.
			*/
			CXChildVisitResult	parse(CXCursor const&			methodCursor,
									  ParsingSettings const&	parsingSettings,
									  PropertyParser&			propertyParser,
									  MethodParsingResult&		out_result)			noexcept;
	};

	#include "Parsing/MethodParser.inl"
}