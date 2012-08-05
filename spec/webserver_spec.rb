require 'socket'

describe :webserver do
  before :all do
    # Spin up the server
    #p = IO.popen('./bin/webserver')
    #@pid = p.pid
  end

  after :all do
    # Kill the server
    #Process.kill('TERM', @pid)
  end

  it 'accepts connections on port 80' do
    expect do
     s = TCPSocket.open('localhost', 80)
     s.should_not be_nil
     s.close
   end .should_not raise_error
  end
end
