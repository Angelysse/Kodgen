/**
*	Copyright (c) 2020 Julien SOYSOUVANH - All Rights Reserved
*
*	This file is part of the Kodgen library project which is released under the MIT License.
*	See the README.md file for full license details.
*/

template <typename FileParserType, typename CodeGenUnitType, typename CodeGenEnvType>
void FileGenerator::processFilesMultithread(FileParserType& fileParser, CodeGenUnitType& codeGenUnit, CodeGenEnvType& env, std::set<fs::path> const& toProcessFiles, FileGenerationResult& out_genResult, uint32 threadCount) const noexcept
{
	ThreadPool								threadPool(threadCount, ETerminationMode::FinishAll);
	std::vector<std::shared_ptr<TaskBase>>	generationTasks;

	//Reserve enough space for all tasks
	generationTasks.reserve(toProcessFiles.size());

	//Launch all parsing -> generation processes
	for (fs::path const& file : toProcessFiles)
	{
		//Add file to the list of parsed files before starting the task to avoid having to synchronize threads
		out_genResult.parsedFiles.push_back(file);

		auto parsingTask = threadPool.submitTask([&](TaskBase*) -> FileParsingResult
												  {
													 //Copy a parser for this task
													 FileParserType		fileParserCopy = fileParser;
													 FileParsingResult	parsingResult;

													 fileParserCopy.parse(file, parsingResult);

													 return parsingResult;
												  });

		generationTasks.emplace_back(threadPool.submitTask([&](TaskBase* parsingTask) -> FileGenerationResult
									 {
										FileGenerationResult out_generationResult;
										
										//Copy the generation unit model to have a fresh one for this generation unit
										CodeGenUnitType	generationUnit = codeGenUnit;

										//Copy the generation environment
										CodeGenEnvType envCopy = env;

										//Get the result of the parsing task
										FileParsingResult parsingResult = TaskHelper::getDependencyResult<FileParsingResult>(parsingTask, 0u);

										//Generate the file if no errors occured during parsing
										if (parsingResult.errors.empty())
										{
											out_generationResult.completed = generationUnit.generateCode(parsingResult, envCopy);
										}
										
										return out_generationResult;
									 
									 }, { parsingTask }));
	}

	//Merge all generation results together
	for (std::shared_ptr<TaskBase>& generationTask : generationTasks)
	{
		out_genResult.mergeResult(TaskHelper::getResult<FileGenerationResult>(generationTask.get()));
	}
}

template <typename FileParserType, typename CodeGenUnitType, typename CodeGenEnvType>
void FileGenerator::processFilesMonothread(FileParserType& fileParser, CodeGenUnitType& codeGenUnit, CodeGenEnvType& env, std::set<fs::path> const& toProcessFiles, FileGenerationResult& out_genResult) const noexcept
{
	for (fs::path const& file : toProcessFiles)
	{
		FileParsingResult parsingResult;

		out_genResult.parsedFiles.push_back(file);

		//Parse file
		if (fileParser.parse(file, parsingResult))
		{
			//Generate file according to parsing result
			out_genResult.completed &= codeGenUnit.generateCode(parsingResult, env);
		}
	}
}

template <typename FileParserType, typename CodeGenUnitType, typename CodeGenEnvType>
FileGenerationResult FileGenerator::generateFiles(FileParserType& fileParser, CodeGenUnitType& codeGenUnit, CodeGenEnvType&	env, bool forceRegenerateAll, uint32 threadCount) noexcept
{
	//Check FileParser validity
	static_assert(std::is_base_of_v<FileParser, FileParserType>, "fileParser type must be a derived class of kodgen::FileParser.");
	static_assert(std::is_copy_constructible_v<FileParserType>, "The provided file parser must be copy-constructible.");

	//Check CodeGenEnv validity
	static_assert(std::is_base_of_v<CodeGenEnv, CodeGenEnvType>, "env type must be a derived class of kodgen::CodeGenEnv.");
	static_assert(std::is_copy_constructible_v<CodeGenEnvType>, "The provided CodeGenEnv must be copy-constructible");

	//Check FileGenerationUnit validity
	static_assert(std::is_base_of_v<CodeGenUnit, CodeGenUnitType>, "codeGenUnit type must be a derived class of kodgen::CodeGenUnit.");
	static_assert(std::is_copy_constructible_v<CodeGenUnitType>, "The CodeGenUnit you provide must be copy-constructible.");

	FileGenerationResult genResult;
	genResult.completed = true;

	if (!checkGenerationSetup(fileParser, codeGenUnit))
	{
		genResult.completed = false;
	}
	else
	{
		//Start timer here
		auto				start			= std::chrono::high_resolution_clock::now();
		std::set<fs::path>	filesToProcess	= identifyFilesToProcess(codeGenUnit, genResult, forceRegenerateAll);

		//Don't setup anything if there are no files to generate
		if (filesToProcess.size() > 0u)
		{
			//Initialize the parsing settings so that compilation arguments are valid.
			//parsingSettings can't be nullptr since it has been checked in the checkGenerationSetup call.
			fileParser.parsingSettings->init(logger);

			generateMacrosFile(*fileParser.parsingSettings, codeGenUnit.getSettings()->getOutputDirectory());

			threadCount = getThreadCount(threadCount);

			//At this point thread count can't be 0
			assert(threadCount > 0);

			if (threadCount == 1u)
			{
				processFilesMonothread(fileParser, codeGenUnit, env, filesToProcess, genResult);
			}
			else
			{
				processFilesMultithread(fileParser, codeGenUnit, env, filesToProcess, genResult, threadCount);
			}
		}

		genResult.duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() / 1000.0f;
	}
	
	return genResult;
}