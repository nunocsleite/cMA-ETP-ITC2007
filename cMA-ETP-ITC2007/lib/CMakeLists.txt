#
# project
#
project(SOlib)

# Include "/lib" directory
include_directories(${PROJECT_SOURCE_DIR})
# Include ParadisEO directories
include_directories(${PROJECT_SOURCE_DIR}/../../ext/include/ParadisEO-2.0/eo/src/)
include_directories(${PROJECT_SOURCE_DIR}/../../ext/include/ParadisEO-2.0/mo/src/)
include_directories(${PROJECT_SOURCE_DIR}/../../ext/include/ParadisEO-2.0/moeo/src/)



add_subdirectory(algorithms)
add_subdirectory(chromosome)
add_subdirectory(containers)
add_subdirectory(data)
add_subdirectory(eval)
add_subdirectory(graphColouring)
add_subdirectory(init)
add_subdirectory(kempeChain)
add_subdirectory(neighbourhood)
add_subdirectory(testset)
add_subdirectory(utils)


#
# Set public header list (add your headers and source files here))
#
set(${PROJECT_NAME}_headers
        # algorithms/eo
        algorithms/eo/Crossover.h
        algorithms/eo/eoAlgoPointer.h
        algorithms/eo/eoCellularEA.h
        algorithms/eo/eoCellularEAMatrix.h
        algorithms/eo/eoDeterministicTournamentSelectorPointer.h
        algorithms/eo/eoGenerationContinuePopVector.h
        algorithms/eo/Mutation.h
        algorithms/eo/eoSelectBestOne.h
        # algorithms/mo
        algorithms/mo/moSimpleCoolingSchedule.h
        algorithms/mo/moTA.h
        algorithms/mo/moTAexplorer.h
        # chromosome
        chromosome/eoChromosome.h
        # containers
        containers/ColumnMatrix.h
        containers/ConflictBasedStatistics.h
        containers/IntMatrix.h
        containers/Matrix.h
        containers/TimetableContainer.h
        containers/TimetableContainerMatrix.h
        containers/VertexPriorityQueue.h
        # data
        data/Constraint.hpp
        data/ConstraintValidator.hpp
        data/Data.h
        data/Exam.h
        data/ITC2007Constraints.hpp
        data/ITC2007Period.h
        data/Period.h
        data/ScheduledExam.h
        data/ScheduledRoom.h
        data/Room.h
        data/TimetableProblemData.hpp
        # eval
        eval/eoETTPEval.h
        eval/eoNumberEvalsCounter.h
        # graphColouring
        graphColouring/GraphColouringHeuristics.h
        # init
        init/ETTPInit.h
        # kempeChain
        kempeChain/ETTPKempeChain.h
        kempeChain/ETTPKempeChainHeuristic.h
        # neighbourhood
        neighbourhood/ETTPneighbor.h
        neighbourhood/ETTPneighborEval.h
        neighbourhood/ETTPneighborhood.h
        neighbourhood/ETTPNeighborhoodExplorer.h
        neighbourhood/ETTPneighborEvalNumEvalsCounter.h
        # testset
        testset/ITC2007TestSet.h
        testset/TestSet.h
        testset/TestSetDescription.h
        # utils
        utils/Common.h
        utils/CurrentDateTime.h
        utils/DateTime.h
)


set(${PROJECT_NAME}_sources
        # chromosome
        chromosome/eoChromosome.cpp
        # containers
        containers/ConflictBasedStatistics.cpp
        containers/TimetableContainerMatrix.cpp
        containers/VertexPriorityQueue.cpp
        # data
        data/TimetableProblemData.cpp
        # graphColouring
        # testset
        testset/ITC2007TestSet.cpp
        testset/TestSet.cpp
        testset/TestSetDescription.cpp
        # utils
        utils/CurrentDateTime.cpp
        utils/Utils.cpp
        utils/Common.cpp
        # lib
        MainApp.cpp
        MainAppITC2007Datasets.cpp
)



add_library(SOlib SHARED ${${PROJECT_NAME}_headers} ${${PROJECT_NAME}_sources})


# Include ParadisEO, Boost Regex, Armadillo, ncurses5-dev libs
#target_link_libraries(${PROJECT_NAME} boost_regex eo es moeo cma eoutils ga armadillo ncurses)
target_link_libraries(${PROJECT_NAME} boost_regex eo es moeo cma eoutils ga armadillo)

