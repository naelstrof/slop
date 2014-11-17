#!/bin/bash
# generateReadme.sh: Regenerates the help section of the README.md using output from ./slop --help.

# Remove help section
sed -i '/^help/,/^```$/d' README.md

# Add the help section again.
echo 'help' >> README.md
echo '----' >> README.md
echo '```text' >> README.md
echo "$(./slop --help)" >> README.md
echo '```' >> README.md
