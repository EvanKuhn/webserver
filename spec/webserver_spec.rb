#===============================================================================
# Webserver tests
#
# Evan Kuhn, 2012-09-09
#===============================================================================
require 'socket'

@@test_port = 8000

def test_port
  #@@test_port += 1
  @@test_port
end

describe :webserver do
  before :each do
    # Spin up the server
    @current_test_port = test_port
    #puts "Starting server on port #{@current_test_port}"
    p = IO.popen("./bin/webserver -p #{@current_test_port}")
    @pid = p.pid
    #puts "Server running, pid = #@pid\n"
    sleep(1)
  end

  after :each do
    # Kill the server
    # TODO - we need the webserver to gracefully catch TERM signals and shut down
    #Process.kill('TERM', @pid)
  end

  it 'accepts connections' do
    expect do
      s = TCPSocket.open('localhost', @current_test_port)
      s.should_not be_nil
      s.close
    end .to_not raise_error
  end

  it 'responds to GET' do
    TCPSocket.open('localhost', @current_test_port) do |s|
      s.send("GET /hello.html HTTP/1.1\r\n\r\n", 0);
      s.read.should ==
        "HTTP/1.0 200 OK\r\n" \
        "Content-Length: 59\r\n" \
        "Content-Type: text/html\r\n" \
        "Server: webserver\r\n" \
        "\r\n" \
        "<html><head></head><body><p>Hello World!</p></body></html>\n"
    end
  end
end
