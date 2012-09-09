#===============================================================================
# Webserver tests
#
# Evan Kuhn, 2012-09-09
#===============================================================================
require 'socket'

TEST_PORT = 8000

describe :webserver do
  before :all do
    # Spin up the server
    p = IO.popen("./bin/webserver -p #{TEST_PORT}")
    @pid = p.pid
    puts "Server running, pid = #@pid\n"
  end

  after :all do
    # Kill the server
    Process.kill('TERM', @pid)
  end

  it 'accepts connections' do
    expect do
     s = TCPSocket.open('localhost', TEST_PORT)
     s.should_not be_nil
     s.close
   end .should_not raise_error
  end
end
