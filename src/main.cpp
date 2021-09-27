#include "Pascal.h"

#include "CLI/CLI.hpp"

int main(int argc, char* argv[]) {
  CLI::App app{"Pascal Compiler and Interpreter"};
  std::string filename;
  bool show_parse_tree = false;
  bool show_reference_listing = false;
  app.require_subcommand(1);
  std::vector<std::string> subprogram_names{"compile", "interpret"};
  std::vector<CLI::App*> subprograms(2, nullptr);
  subprograms[0] = app.add_subcommand(subprogram_names[0], "Compile the Pascal source code");
  subprograms[1] = app.add_subcommand(subprogram_names[1], "Execute the Pascal source code");
  for (auto &i : subprograms) {
    i->add_option("-f,--filename", filename, "Input filename");
    i->add_flag("-i", show_parse_tree, "Show the parse tree");
    i->add_flag("-x", show_reference_listing, "Show the reference listing");
  }
  CLI11_PARSE(app, argc, argv);
  std::string flags;
  for (size_t i = 0; i < subprograms.size(); ++i) {
    if (subprograms[i]->parsed()) {
      if (show_parse_tree) flags += "i";
      if (show_reference_listing) flags += "x";
      Pascal p(subprogram_names[i], filename, flags);
      break;
    }
  }
  return 0;
}
