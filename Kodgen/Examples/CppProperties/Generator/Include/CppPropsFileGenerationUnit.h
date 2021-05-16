#pragma once

#include <Kodgen/Misc/Filesystem.h>
#include <Kodgen/CodeGen/FileGenerationUnit.h>
#include <Kodgen/CodeGen/GeneratedFile.h>

class CppPropsFileGenerationUnit : public kodgen::FileGenerationUnit
{
	private:
		fs::path makeGeneratedFilePath(fs::path const& sourceFilePath) const noexcept;

	protected:
		virtual void generateCodeInternal(kodgen::FileParsingResult const& parsingResult, kodgen::FileGenerationResult& out_genResult) noexcept override;

	public:
		virtual bool isUpToDate(fs::path const& sourceFile)	const noexcept override;
};