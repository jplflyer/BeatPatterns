#include <iostream>
#include <beat_patterns/CLI.h>
#include <beat_patterns/Preferences.h>

/**
 * Entrypoint.
 */
int main(int argc, char **argv) {
    BeatPatterns::Preferences::setupForCLI();
    BeatPatterns::CLI cli;
    cli.parseArgs(argc, argv);
    cli.run();
}
