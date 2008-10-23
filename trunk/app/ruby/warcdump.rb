#!/usr/bin/env ruby

$: << "../../lib/private/plugin/ruby"
$: << "/usr/local/warc-tools/ruby"

require 'warctools'
require 'wfile'
require 'wrecord'

include Warctools

def dumpWarc(filename)


  w = WFile.new(filename, 1024 * 1024 * 1024,
                Warctools::WARC_FILE_READER, 
                Warctools::WARC_FILE_DETECT_COMPRESSION, ".")


  puts "%-16s %-12s %-22s %-50s" % ["Time Stamp", "Offset", "Content Type", "Url"]
  while (w.hasMoreRecords()) do
    r =  w.nextRecord()

    puts "%-16s %-12s %-22s% -50s" % [r.getDate, "#{r.getOffset}", r.getContentType, r.getTargetUri]

    r.destroy()
  end
end

def main()

  if (ARGV.size ==0)
    puts "Usage: #{__FILE__} warcfile"
  end

  dumpWarc(ARGV[0])
end

main()
