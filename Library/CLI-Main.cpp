#include <iostream>
#include <song_editor/CLI.h>
#include <song_editor/Preferences.h>

/**
 * Entrypoint.
 */
int main(int argc, char **argv) {
    SongEditor::Preferences::setupForCLI();
    SongEditor::CLI cli;
    cli.parseArgs(argc, argv);
    cli.run();
}
