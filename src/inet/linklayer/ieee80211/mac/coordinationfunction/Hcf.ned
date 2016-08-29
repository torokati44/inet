//
// Copyright (C) 2016 OpenSim Ltd.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see http://www.gnu.org/licenses/.
// 

package inet.linklayer.ieee80211.mac.coordinationfunction;

import inet.linklayer.ieee80211.mac.blockack.OriginatorBlockAckAgreementPolicy;
import inet.linklayer.ieee80211.mac.blockack.RecipientBlockAckAgreementPolicy;
import inet.linklayer.ieee80211.mac.channelaccess.Edca;
import inet.linklayer.ieee80211.mac.channelaccess.Hcca;
import inet.linklayer.ieee80211.mac.contract.IRateControl;
import inet.linklayer.ieee80211.mac.originator.NonQoSRecoveryProcedure;
import inet.linklayer.ieee80211.mac.originator.OriginatorQoSAckPolicy;
import inet.linklayer.ieee80211.mac.originator.OriginatorQoSMacDataService;
import inet.linklayer.ieee80211.mac.originator.QoSRecoveryProcedure;
import inet.linklayer.ieee80211.mac.originator.QoSRtsPolicy;
import inet.linklayer.ieee80211.mac.originator.TxopProcedure;
import inet.linklayer.ieee80211.mac.protectionmechanism.SingleProtectionMechanism;
import inet.linklayer.ieee80211.mac.rateselection.QoSRateSelection;
import inet.linklayer.ieee80211.mac.recipient.QoSCtsPolicy;
import inet.linklayer.ieee80211.mac.recipient.RecipientQoSAckPolicy;
import inet.linklayer.ieee80211.mac.recipient.RecipientQoSMacDataService;


//
// HCF -- Hybrid Coordination Function
//
module Hcf
{
    parameters:
        int numEdcafs = default(4);
        int maxQueueSize = default(100);
        string rateControlType = default("");

        string rxModule;
        string txModule;

        *.rateSelectionModule = "^.rateSelection";
        *.rxModule = "^." + rxModule;

        @class(Hcf);
        @display("i=block/queue;bgb=1311.142,783.852");

    submodules:

        edca: Edca {
            numEdcafs = numEdcafs;
            @display("p=83.83125,230.125;i=block/star");
        }
        hcca: Hcca {
            @display("p=83.83125,368.2;i=block/star");
        }

        rateSelection: QoSRateSelection {
            parameters:
                rateControlModule = "^.rateControl";
                @display("p=170.95001,534.21875;i=block/cogwheel");
        }
        rateControl: <rateControlType> like IRateControl if rateControlType != "" {
            parameters:
                @display("p=170.95001,675.5813");
        }

        originatorMacDataService: OriginatorQoSMacDataService {
            @display("p=522.7125,534.21875;i=block/fork");
        }
        recipientMacDataService: RecipientQoSMacDataService {
            @display("p=522.7125,675.5813;i=block/join");
        }
        singleProtectionMechanism: SingleProtectionMechanism {
            @display("p=220.26251,69.0375;i=block/encrypt");
        }
        edcaMgmtAndNonQoSRecoveryProcedure: NonQoSRecoveryProcedure {
            cwCalculatorModule = "^.edca.edcaf[1]"; // best effort
            rtsPolicyModule = "^.rtsPolicy";
            @display("p=616.40625,304.09375;i=block/cogwheel");
        }
        edcaDataRecoveryProcedures[numEdcafs]: QoSRecoveryProcedure {
            cwCalculatorModule = "^.edca.edcaf[" + string(index) + "]";
            rtsPolicyModule = "^.rtsPolicy";
            @display("p=616.40625,187.38751,row,50;i=block/cogwheel");
        }
        edcaTxopProcedures[numEdcafs]: TxopProcedure {
            @display("p=616.40625,78.9,row,50;i=block/timer");
        }

        recipientAckPolicy: RecipientQoSAckPolicy {
            @display("p=1017.48126,187.38751;i=block/control");
        }
        originatorAckPolicy: OriginatorQoSAckPolicy {
            @display("p=1017.48126,78.9");
        }
        rtsPolicy: QoSRtsPolicy {
            @display("p=1017.48126,304.09375;i=block/control");
        }
        ctsPolicy: QoSCtsPolicy {
            @display("p=1017.48126,424.08752;i=block/control");
        }
        originatorBlockAckAgreementPolicy: OriginatorBlockAckAgreementPolicy {
            originatorAckPolicyModule = "^.originatorAckPolicy";
            @display("p=1017.48126,535.8625;i=block/control");
        }
        recipientBlockAckAgreementPolicy: RecipientBlockAckAgreementPolicy {
            @display("p=1017.48126,652.5688;i=block/control");
        }

}
