Crossing Condition: if the Incoming bid catches up to or surpasses the closest resting bid transaction will happen.

Fill Order: If incoming bid crosses multiple restings bids it will transact with the closest AND earliest first.

Partial vs Full Fill: If incoming offer passes closest price point it and the quantity of stocks, incoming will consume the next resting bid if its still smaller than the incoming offer. Or if it can't exceed the quantity it will not consume all of the resting bid.

Case A — resting ≥ incoming: the resting order covers the whole incoming order. Trade the incoming amount, shrink the resting order by that much (it stays), incoming is done.

Case B — resting < incoming: the resting order isn't enough. Trade all of it, remove it, incoming still has leftover → go to the next resting order.

Leftover: If incoming bid can cross all the available crossed sells, left over incoming order will be written to bids/asks order in the amount of what has left.

End of Transaction: After the transaction finishes best buy must be strictly less than cheapest sell. transaction should continue until this rule is set or the when no more sell/buy is left   .


