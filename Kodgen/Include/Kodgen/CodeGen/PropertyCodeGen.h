/**
*	Copyright (c) 2020 Julien SOYSOUVANH - All Rights Reserved
*
*	This file is part of the Kodgen library project which is released under the MIT License.
*	See the README.md file for full license details.
*/

#pragma once

#include <string>

#include "Kodgen/Config.h"
#include "Kodgen/Misc/FundamentalTypes.h"
#include "Kodgen/CodeGen/CodeGenEnv.h"
#include "Kodgen/InfoStructures/EntityInfo.h"

namespace kodgen
{
	class PropertyCodeGen
	{
		protected:
			/**
			*	@brief Check if 2 entity types masks have at least one common entity type.
			* 
			*	@param lhs First entity type mask.
			*	@param rhs Second entity type mask.
			* 
			*	@return true if at least one entity type is common to the 2 provided masks, else false.
			*/
			static inline bool entityTypeOverlap(EEntityType lhs, EEntityType rhs) noexcept;

		public:
			virtual ~PropertyCodeGen() = default;

			/**
			*	@brief	Initialize the property code gen and the provided environment. This method must make sure that the provided
			*			environment is valid to generate code with (i.e. that the environment inherits from all necessary class(es)
			*			for this property to work properly).
			*			If the property doesn't require any specific environment, can return true right away.
			*			The method is called by CodeGenModule::initialize before any call to PropertyCodeGen::generateCode.
			*
			*	@param env Generation environment.
			* 
			*	@return true if the environment is valid and initialization completed successfully, else false.
			*/
			virtual bool	initialize(CodeGenEnv& env)								const	noexcept = 0;

			/**
			*	@brief Generate code for a given entity.
			*	
			*	@param entity			Entity to generate code for.
			*	@param property			Property that triggered the property generation.
			*	@param propertyIndex	Index of the property in the entity's propertyGroup.
			*	@param env				Generation environment structure.
			*	@param inout_result		String the method should append the generated code to.
			*	
			*	@return true if the generation completed successfully, else false.
			*/
			virtual bool	generateCode(EntityInfo const&	entity,
										 Property const&	property,
										 uint8				propertyIndex,
										 CodeGenEnv&		env,
										 std::string&		inout_result)			const	noexcept = 0;

			/**
			*	@brief Check if this property should generate code for the provided entity/property pair.
			*
			*	@param entity			Checked entity.
			*	@param property			Checked property.
			*	@param propertyIndex	Index of the property in the entity's propertyGroup.
			*
			*	@return true if this property should generate code for the provided entity, else false.
			*/
			virtual bool	shouldGenerateCode(EntityInfo const&	entity,
											   Property const&		property,
											   uint8				propertyIndex)	const	noexcept = 0;
	};

	#include "Kodgen/CodeGen/PropertyCodeGen.inl"
}