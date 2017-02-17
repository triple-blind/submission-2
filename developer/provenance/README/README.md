provenance
==========

Be sure to see the project's web page at [http://collberg.github.io/provenance/](http://collberg.github.io/provenance/)
for binary installation instructions and to see what other materials are required
for development in addition to the source code.

There might be four branches visible:

master - Files in this branch should compile and run on all three platforms.
Furthermore, updates should be tagged and collected into a release with both source
and binary distributions.

development - New development takes place on this branch.  Each update should relate
to a specific development task such as the addition of new Provenance events.  Changes
likely only run on the platform which is was developed on, at least initially.  It may
not build on the others.  There is no binary distribution.  It is left up to the
individual developer to perform the build.

backup - This is a nightly or as needed update of the files on the main development
computer.  It can be used to exchange files with other developers or just provide
a redundant copy of the files.

gh-pages - The files here form the view that is observed at the URL listed above.
