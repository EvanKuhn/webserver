require 'rake'
require "rspec/core/rake_task"

desc "Clean up from a build"
task :clean do
  sh 'make clean'
end

desc "Build the webserver binaries"
task :build do
  sh 'make'
end

desc "Run C unit tests"
task :test do
  sh './bin/run_tests'
end

desc "Run RSpec tests"
RSpec::Core::RakeTask.new(:spec) do |t|
  t.pattern = FileList['spec/*_spec.rb']
  t.rspec_opts = '-cfd'
end

#task :default => :spec
