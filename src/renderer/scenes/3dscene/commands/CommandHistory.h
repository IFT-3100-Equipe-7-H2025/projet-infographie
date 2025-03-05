#pragma once

#include "3dscene/commands/Command.h"
#include <memory>
#include <vector>

// Maximum number of commands that can be stored in the undo/redo stacks, after which the oldest command is removed.
//   This is necessary to prevent storing an infinite number of commands in memory since they might contain shared pointers
//   to nodes and other objects that would prevent them from being deleted otherwise.
constexpr int MAX_COMMAND_HISTORY = 100;

class CommandHistory
{
public:
    CommandHistory() : undoStack(), redoStack() {}

    void executeCommand(std::shared_ptr<Command> command);
    void undo();
    void redo();

    [[nodiscard]] const std::vector<std::shared_ptr<Command>>& GetUndoStack() { return undoStack; }
    [[nodiscard]] const std::vector<std::shared_ptr<Command>>& GetRedoStack() { return redoStack; }

private:
    std::vector<std::shared_ptr<Command>> undoStack;
    std::vector<std::shared_ptr<Command>> redoStack;
};
